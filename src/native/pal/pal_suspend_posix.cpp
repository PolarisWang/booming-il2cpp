// pal_suspend_posix.cpp — POSIX stub: hard suspension not supported outside Windows
//
// CoreCLR also gates SuspendThread behind `#ifndef DISABLE_THREADSUSPEND` and
// explicitly comments "On non-Windows CORECLR platforms remove Thread::SuspendThread
// support".  On POSIX (Linux, macOS, Android) we use SIGUSR2 trampoline / pure
// soft rendezvous instead — matching the PalPreemptRequest abstraction.

#include <chaos/pal/pal_suspend.h>

namespace chaos::il2cpp::pal {

bool PalHardSuspendSupported() noexcept {
    return false;
}

SuspendResult PalSuspendThread(void* /*os_handle*/) noexcept {
    return SuspendResult::kUnsupported;
}

SuspendResult PalResumeThread(void* /*os_handle*/) noexcept {
    return SuspendResult::kUnsupported;
}

SuspendResult PalGetThreadContext(void* /*os_handle*/, uint64_t* /*ip_out*/,
                                  uint64_t* /*sp_out*/, uint64_t* /*gpr_out*/) noexcept {
    return SuspendResult::kUnsupported;
}

}  // namespace chaos::il2cpp::pal