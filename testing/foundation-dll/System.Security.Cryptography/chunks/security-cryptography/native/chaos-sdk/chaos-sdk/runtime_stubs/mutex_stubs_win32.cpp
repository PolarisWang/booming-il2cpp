// mutex_stubs_win32.cpp — Win32 System.Threading.Mutex implementation
//
// Managed Mutex wraps an OS synchronization handle.  On Windows this is a
// HANDLE to a kernel Mutex (CreateMutexW).

#include <chaos/native_types.h>
#include "generated_code_compat.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Helper: extract UTF-8 string from a managed string pointer ───────────
static const char* GetManagedStringUtf8(CHAOS_IL2CPP_INTPTR name_ptr) noexcept {
    if (name_ptr == 0) return nullptr;
    auto* str = reinterpret_cast<const chaos_managed_string*>(name_ptr);
    return (str != nullptr && str->length > 0) ? str->utf8_data : nullptr;
}

// ── chaos_mutex_create ─────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR chaos_mutex_create(
    CHAOS_IL2CPP_INT32 initially_owned,
    CHAOS_IL2CPP_INTPTR name_ptr,
    CHAOS_IL2CPP_INTPTR* created_new) noexcept
{
    const char* name_utf8 = GetManagedStringUtf8(name_ptr);

    // Windows: use CreateMutexW (wide char).
    // Convert UTF-8 name to wide char if provided.
    wchar_t wide_name[260];
    wchar_t* name_wide = nullptr;
    if (name_utf8 != nullptr) {
        int len = MultiByteToWideChar(CP_UTF8, 0, name_utf8, -1, nullptr, 0);
        if (len > 0 && len < 260) {
            MultiByteToWideChar(CP_UTF8, 0, name_utf8, -1, wide_name, len);
            name_wide = wide_name;
        }
    }

    HANDLE h = CreateMutexW(nullptr, (initially_owned != 0), name_wide);
    if (h == nullptr) return 0;

    if (created_new != nullptr) {
        *created_new = (GetLastError() != ERROR_ALREADY_EXISTS) ? 1 : 0;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(h));
}

// ── chaos_mutex_open ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name_ptr) noexcept
{
    const char* name_utf8 = GetManagedStringUtf8(name_ptr);
    if (name_utf8 == nullptr) return 0;

    wchar_t wide_name[260];
    int len = MultiByteToWideChar(CP_UTF8, 0, name_utf8, -1, nullptr, 0);
    if (len <= 0 || len >= 260) return 0;
    MultiByteToWideChar(CP_UTF8, 0, name_utf8, -1, wide_name, len);

    HANDLE h = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, wide_name);
    return (h != nullptr)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(h))
        : 0;
}

// ── chaos_mutex_release ────────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;
    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    return ReleaseMutex(h) ? 0 : -1;
}

// ── chaos_mutex_wait_one ───────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == 0) return -1;

    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD result = WaitForSingleObject(h, ms);
    switch (result) {
        case WAIT_OBJECT_0:  return 1;   // Acquired.
        case WAIT_TIMEOUT:   return 0;   // Timeout.
        case WAIT_ABANDONED: return 1;   // Acquired (abandoned — former owner terminated).
        default:             return -1;  // Error.
    }
}

// ── chaos_mutex_close ──────────────────────────────────────────────────
CHAOS_IL2CPP_INT32 chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;
    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    return CloseHandle(h) ? 0 : -1;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
