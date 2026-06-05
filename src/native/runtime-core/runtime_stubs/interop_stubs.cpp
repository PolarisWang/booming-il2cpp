// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// interop_stubs.cpp — Simple interop/Marshal stub implementations
// These stubs are compiled from source (not part of prebuilt lib)
// to avoid stale-symbol issues with the SDK runtime library.
#include <cstdlib>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/interop_stubs.h"
#include "runtime_core.h"
#include "engine_binding.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Marshal P/Invoke error helpers ─────────────────────────────
// These delegate to the runtime's ThreadState-based storage.
// Used by Marshal.SetLastPInvokeError / GetLastPInvokeError.

void ChaosMarshalSetLastPInvokeError(CHAOS_IL2CPP_INT32 error) noexcept
{
    auto* ts = GetCurrentThreadState();
    SetLastPInvokeError(ts, error);
}

CHAOS_IL2CPP_INT32 ChaosMarshalGetLastPInvokeError(void) noexcept
{
    auto* ts = GetCurrentThreadState();
    return GetLastPInvokeError(ts);
}

// ── Exception code/pointers stubs ──────────────────────────────
// These are Windows SEH intrinsics on .NET. Return safe defaults
// for cross-platform AOT. Full SEH integration would require
// __try/__except in the generated code, which is not feasible
// in the current AOT model.

CHAOS_IL2CPP_INT32 ChaosMarshalGetExceptionCode(void) noexcept
{
    // Stub: no active SEH handler in AOT-generated code.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionPointers(void) noexcept
{
    // Stub: no active SEH handler in AOT-generated code.
    return 0;
}

// ── GetHRForLastWin32Error ──────────────────────────────────
// Converts the last P/Invoke error to an HRESULT.
// .NET semantics: error <= 0 ? error : (error & 0x0000FFFF) | 0x80070000
CHAOS_IL2CPP_INT32 ChaosMarshalGetHRForLastWin32Error(void) noexcept
{
    auto* ts = GetCurrentThreadState();
    auto error = GetLastPInvokeError(ts);
    return error <= 0 ? error : (error & 0x0000FFFF) | static_cast<CHAOS_IL2CPP_INT32>(0x80070000);
}

// ── COM cleanup check stub ────────────────────────────────────
// The runtime has no COM-visible RCW tracking in AOT mode,
// so no objects are pending cleanup.

CHAOS_IL2CPP_INT32 ChaosMarshalAreComObjectsAvailableForCleanup(void) noexcept
{
    return 0;  // false
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
