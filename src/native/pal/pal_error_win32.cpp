// pal_error_win32.cpp — Win32 error/process/thread info implementation

#include <chaos/pal/pal_error.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace chaos::il2cpp::pal {

int32_t PalGetLastError() noexcept {
    return static_cast<int32_t>(::GetLastError());
}

void PalSetLastError(int32_t error) noexcept {
    ::SetLastError(static_cast<DWORD>(error));
}

int32_t PalGetCurrentProcessId() noexcept {
    return static_cast<int32_t>(::GetCurrentProcessId());
}

uint64_t PalGetCurrentThreadId() noexcept {
    return static_cast<uint64_t>(::GetCurrentThreadId());
}

intptr_t PalGetCurrentProcess() noexcept {
    return reinterpret_cast<intptr_t>(::GetCurrentProcess());
}

intptr_t PalGetCurrentThread() noexcept {
    return reinterpret_cast<intptr_t>(::GetCurrentThread());
}

}  // namespace chaos::il2cpp::pal
