// pal_suspend_win32.cpp — Win32 hard suspension (SuspendThread + GetThreadContext + ResumeThread)
//
// Windows is the ONLY platform with a reliable SuspendThread, and even then
// the context returned by GetThreadContext is NOT guaranteed to be consistent
// (CoreCLR threadsuspend.cpp:3111).  We follow CoreCLR's pattern: suspend briefly
// to redirect, then resume — never scan from a physically suspended thread.

#include <chaos/pal/pal_suspend.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

bool PalHardSuspendSupported() noexcept {
    return true;
}

SuspendResult PalSuspendThread(void* os_handle) noexcept {
    if (!os_handle) return SuspendResult::kNotFound;
    HANDLE h = static_cast<HANDLE>(os_handle);
    DWORD ret = ::SuspendThread(h);
    if (ret == (DWORD)-1) {
        DWORD err = ::GetLastError();
        if (err == ERROR_ACCESS_DENIED) return SuspendResult::kForbidden;
        return SuspendResult::kFailure;
    }
    if (ret > 0) {
        // Thread was already suspended (count > 0).  Count the suspension
        // as a success — the caller will resume once.
        return SuspendResult::kSuspended;
    }
    return SuspendResult::kSuccess;
}

SuspendResult PalResumeThread(void* os_handle) noexcept {
    if (!os_handle) return SuspendResult::kNotFound;
    HANDLE h = static_cast<HANDLE>(os_handle);
    DWORD ret = ::ResumeThread(h);
    if (ret == (DWORD)-1) {
        return SuspendResult::kFailure;
    }
    return SuspendResult::kSuccess;
}

SuspendResult PalGetThreadContext(void* os_handle, uint64_t* ip_out,
                                  uint64_t* sp_out, uint64_t* gpr_out) noexcept {
    if (!os_handle) return SuspendResult::kNotFound;
    HANDLE h = static_cast<HANDLE>(os_handle);

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
    if (!::GetThreadContext(h, &ctx)) {
        return SuspendResult::kFailure;
    }

    // x64: RIP = instruction pointer, RSP = stack pointer
    if (ip_out) *ip_out = ctx.Rip;
    if (sp_out) *sp_out = ctx.Rsp;

    // Fill the 16-entry GPR array (RAX=0, RCX=1, RDX=2, RBX=3,
    // RSP=4, RBP=5, RSI=6, RDI=7, R8=8..R15=15).
    if (gpr_out) {
        gpr_out[0]  = ctx.Rax;
        gpr_out[1]  = ctx.Rcx;
        gpr_out[2]  = ctx.Rdx;
        gpr_out[3]  = ctx.Rbx;
        gpr_out[4]  = ctx.Rsp;
        gpr_out[5]  = ctx.Rbp;
        gpr_out[6]  = ctx.Rsi;
        gpr_out[7]  = ctx.Rdi;
        gpr_out[8]  = ctx.R8;
        gpr_out[9]  = ctx.R9;
        gpr_out[10] = ctx.R10;
        gpr_out[11] = ctx.R11;
        gpr_out[12] = ctx.R12;
        gpr_out[13] = ctx.R13;
        gpr_out[14] = ctx.R14;
        gpr_out[15] = ctx.R15;
    }

    return SuspendResult::kSuccess;
}

}  // namespace chaos::il2cpp::pal