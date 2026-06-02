// mutex_stubs_posix.cpp — POSIX System.Threading.Mutex implementation
//
// Managed Mutex wraps an OS synchronization handle.  On POSIX, named mutexes
// use named semaphores (sem_open) while unnamed mutexes use pthread_mutex_t.
//
// .NET semantics: Mutex.WaitOne returns when the mutex is acquired;
// Mutex.ReleaseMutex releases ownership.  Named mutexes are visible
// system-wide and can synchronize across processes.

#include <chaos/native_types.h>
#include "generated_code_compat.h"

#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Helper: extract UTF-8 string from a managed string pointer ───────────
static const char* GetManagedStringUtf8(CHAOS_IL2CPP_INTPTR name_ptr) noexcept {
    if (name_ptr == 0) return nullptr;
    auto* str = reinterpret_cast<const chaos_managed_string*>(name_ptr);
    return (str != nullptr && str->length > 0) ? str->utf8_data : nullptr;
}

// ── Helper: sanitise name for POSIX named semaphore ──────────────────────
// POSIX named semaphores require a leading '/' and must not contain slashes.
// We replace '/' with '_' and ensure the result starts with '/'.
static void SanitiseSemName(const char* src, char* dst, size_t dst_size) noexcept {
    if (src == nullptr || dst_size == 0) { if (dst_size > 0) dst[0] = '\0'; return; }
    size_t j = 0;
    // Ensure leading '/'
    if (src[0] != '/') {
        if (j + 1 < dst_size) dst[j++] = '/';
    }
    for (size_t i = 0; src[i] != '\0' && j + 1 < dst_size; i++) {
        char c = src[i];
        // Replace '/' with '_' (POSIX sem name must not contain '/')
        if (c == '/') c = '_';
        dst[j++] = c;
    }
    dst[j] = '\0';
}

// ── Bookkeeping for unnamed pthread mutexes ──────────────────────────────
// Named mutexes use sem_open which is self-managing.  Unnamed mutexes
// allocate a pthread_mutex_t on the heap and track it in a global table.
namespace {
std::mutex s_unnamed_mutex_table_lock;
std::unordered_map<CHAOS_IL2CPP_INTPTR, pthread_mutex_t*> s_unnamed_mutexes;
CHAOS_IL2CPP_INTPTR s_next_unnamed_handle = 0x10000000;

CHAOS_IL2CPP_INTPTR StoreUnnamedMutex(pthread_mutex_t* mtx) noexcept {
    std::lock_guard<std::mutex> lock(s_unnamed_mutex_table_lock);
    CHAOS_IL2CPP_INTPTR handle = s_next_unnamed_handle++;
    if (handle == 0) handle = s_next_unnamed_handle++;
    s_unnamed_mutexes[handle] = mtx;
    return handle;
}

pthread_mutex_t* LookupUnnamedMutex(CHAOS_IL2CPP_INTPTR handle) noexcept {
    std::lock_guard<std::mutex> lock(s_unnamed_mutex_table_lock);
    auto it = s_unnamed_mutexes.find(handle);
    return (it != s_unnamed_mutexes.end()) ? it->second : nullptr;
}

pthread_mutex_t* RemoveUnnamedMutex(CHAOS_IL2CPP_INTPTR handle) noexcept {
    std::lock_guard<std::mutex> lock(s_unnamed_mutex_table_lock);
    auto it = s_unnamed_mutexes.find(handle);
    if (it == s_unnamed_mutexes.end()) return nullptr;
    auto* mtx = it->second;
    s_unnamed_mutexes.erase(it);
    return mtx;
}
}  // anonymous namespace

// ── chaos_mutex_create ─────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR chaos_mutex_create(
    CHAOS_IL2CPP_INT32 initially_owned,
    CHAOS_IL2CPP_INTPTR name_ptr,
    CHAOS_IL2CPP_INTPTR* created_new) noexcept
{
    const char* name_utf8 = GetManagedStringUtf8(name_ptr);

    if (name_utf8 != nullptr) {
        // Named mutex: use named semaphore (sem_open).
        char sem_name[256];
        SanitiseSemName(name_utf8, sem_name, sizeof(sem_name));

        // sem_open with O_CREAT | O_EXCL to detect creation vs open.
        // Try O_EXCL first; if EEXIST, fall back to plain O_CREAT.
        sem_t* sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, initially_owned ? 0 : 1);
        bool is_new = true;
        if (sem == SEM_FAILED && errno == EEXIST) {
            sem = sem_open(sem_name, O_CREAT, 0666, 1);
            is_new = false;
        }
        if (sem == SEM_FAILED) return 0;

        if (created_new != nullptr) {
            *created_new = is_new ? 1 : 0;
        }

        return static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(sem));
    } else {
        // Unnamed mutex: use pthread_mutex_t.
        auto* mtx = new pthread_mutex_t();
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
        int rc = pthread_mutex_init(mtx, &attr);
        pthread_mutexattr_destroy(&attr);
        if (rc != 0) {
            delete mtx;
            return 0;
        }

        CHAOS_IL2CPP_INTPTR handle = StoreUnnamedMutex(mtx);

        if (initially_owned) {
            pthread_mutex_lock(mtx);
        }

        if (created_new != nullptr) {
            *created_new = 1;
        }

        return handle;
    }
}

// ── chaos_mutex_open ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name_ptr) noexcept
{
    const char* name_utf8 = GetManagedStringUtf8(name_ptr);
    if (name_utf8 == nullptr) return 0;

    char sem_name[256];
    SanitiseSemName(name_utf8, sem_name, sizeof(sem_name));

    sem_t* sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(sem));
}

// ── chaos_mutex_release ────────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;

    if (handle >= 0x10000000) {
        // Unnamed mutex address range.
        auto* mtx = LookupUnnamedMutex(handle);
        if (mtx == nullptr) return -1;
        return (pthread_mutex_unlock(mtx) == 0) ? 0 : -1;
    } else {
        // Named semaphore.
        auto* sem = reinterpret_cast<sem_t*>(static_cast<uintptr_t>(handle));
        return (sem_post(sem) == 0) ? 0 : -1;
    }
}

// ── chaos_mutex_wait_one ───────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == 0) return -1;

    if (handle >= 0x10000000) {
        // Unnamed mutex.
        auto* mtx = LookupUnnamedMutex(handle);
        if (mtx == nullptr) return -1;

        if (timeout_ms == 0) {
            return (pthread_mutex_trylock(mtx) == 0) ? 1 : 0;
        } else if (timeout_ms < 0) {
            return (pthread_mutex_lock(mtx) == 0) ? 1 : -1;
        } else {
            // POSIX has no timed mutex lock without cond var trickery.
            // Fall back to trylock in a spin loop with yield (not ideal but
            // acceptable for the rare-use P/Invoke path).
            auto deadline = std::chrono::steady_clock::now()
                + std::chrono::milliseconds(timeout_ms);
            do {
                if (pthread_mutex_trylock(mtx) == 0) return 1;
                std::this_thread::yield();
            } while (std::chrono::steady_clock::now() < deadline);
            return 0;
        }
    } else {
        // Named semaphore.
        auto* sem = reinterpret_cast<sem_t*>(static_cast<uintptr_t>(handle));
        if (timeout_ms == 0) {
            return (sem_trywait(sem) == 0) ? 1 : 0;
        } else if (timeout_ms < 0) {
            return (sem_wait(sem) == 0) ? 1 : -1;
        } else {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L) {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000L;
            }
            return (sem_timedwait(sem, &ts) == 0) ? 1 : (errno == ETIMEDOUT ? 0 : -1);
        }
    }
}

// ── chaos_mutex_close ──────────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;

    if (handle >= 0x10000000) {
        // Unnamed mutex: destroy and free.
        auto* mtx = RemoveUnnamedMutex(handle);
        if (mtx == nullptr) return -1;
        pthread_mutex_destroy(mtx);
        delete mtx;
        return 0;
    } else {
        // Named semaphore: close the sem_open handle.
        auto* sem = reinterpret_cast<sem_t*>(static_cast<uintptr_t>(handle));
        return (sem_close(sem) == 0) ? 0 : -1;
    }
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
