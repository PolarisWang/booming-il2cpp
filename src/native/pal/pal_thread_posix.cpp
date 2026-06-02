// pal_thread_posix.cpp — POSIX threading primitives (pthreads)

#include <chaos/pal/pal_thread.h>

#include <pthread.h>
#include <sys/resource.h>
#include <time.h>
#include <cerrno>
#include <cstdlib>
#include <new>

namespace chaos::il2cpp::pal {

PalThread* PalThreadCreate(PalThreadProc proc, void* arg) noexcept {
    auto* t = new (std::nothrow) pthread_t();
    if (!t) return nullptr;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int rc = pthread_create(t, &attr, proc, arg);
    pthread_attr_destroy(&attr);
    if (rc != 0) {
        delete t;
        return nullptr;
    }
    return reinterpret_cast<PalThread*>(t);
}

void* PalThreadJoin(PalThread* thread) noexcept {
    if (!thread) return nullptr;
    void* result = nullptr;
    pthread_join(*reinterpret_cast<pthread_t*>(thread), &result);
    delete reinterpret_cast<pthread_t*>(thread);
    return result;
}

void PalSleepMs(uint64_t ms) noexcept {
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(ms / 1000);
    ts.tv_nsec = static_cast<long>(ms % 1000) * 1'000'000L;
    // nanosleep may be interrupted by signals; retry if so.
    while (::nanosleep(&ts, &ts) == -1 && errno == EINTR) {}
}

void PalYield() noexcept {
    ::sched_yield();
}

void PalGetStackBounds(void*& out_base, void*& out_limit) noexcept {
#if defined(__APPLE__)
    out_base  = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pthread_get_stackaddr_np(pthread_self())));
    out_limit = static_cast<char*>(out_base) - pthread_get_stacksize_np(pthread_self());
#else
    pthread_attr_t attr;
    void* stack_addr;
    size_t stack_size;
    if (pthread_getattr_np(pthread_self(), &attr) == 0) {
        if (pthread_attr_getstack(&attr, &stack_addr, &stack_size) == 0) {
            out_base  = static_cast<char*>(stack_addr) + stack_size;
            out_limit = stack_addr;
        }
        pthread_attr_destroy(&attr);
    }
#endif
}

void* PalDuplicateCurrentThreadHandle() noexcept {
    // POSIX uses os_thread_id for signal-based preemption; no handle needed.
    return nullptr;
}

void PalCloseThreadHandle(void* handle) noexcept {
    // No-op on POSIX; handle is always nullptr.
    (void)handle;
}

bool PalSetThreadPriority(int level) noexcept {
    // POSIX: map managed priority level to nice value (-20..19 for SCHED_OTHER).
    // SCHED_OTHER only allows nice-based prioritization within the same
    // scheduling policy; root privileges are NOT required for nice values.
    static constexpr int kNiceMap[] = {
        19,   // 0 = Lowest
        10,   // 1 = BelowNormal
        0,    // 2 = Normal
        -10,  // 3 = AboveNormal
        -20,  // 4 = Highest
    };
    if (level < 0 || level > 4) level = 2;
    errno = 0;
    return ::setpriority(PRIO_PROCESS, 0, kNiceMap[level]) == 0;
}

}  // namespace chaos::il2cpp::pal
