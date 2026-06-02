// pal_error_posix.cpp — POSIX error/process/thread info stubs

#include <chaos/pal/pal_error.h>

#include <unistd.h>
#include <sys/syscall.h>

namespace chaos::il2cpp::pal {

int32_t PalGetLastError() noexcept {
    return 0;  // POSIX: no Win32-style last-error concept.
}

void PalSetLastError(int32_t /*error*/) noexcept {
    // No-op on POSIX.
}

int32_t PalGetCurrentProcessId() noexcept {
    return static_cast<int32_t>(::getpid());
}

uint64_t PalGetCurrentThreadId() noexcept {
#if defined(__linux__)
    return static_cast<uint64_t>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
    uint64_t tid;
    ::pthread_threadid_np(nullptr, &tid);
    return tid;
#else
    return static_cast<uint64_t>(reinterpret_cast<intptr_t>(pthread_self()));
#endif
}

intptr_t PalGetCurrentProcess() noexcept {
    return 0;  // No opaque process handle on POSIX.
}

intptr_t PalGetCurrentThread() noexcept {
    return 0;  // No opaque thread handle on POSIX.
}

}  // namespace chaos::il2cpp::pal
