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
    if (ts == nullptr) return;
    SetLastPInvokeError(ts, error);
}

CHAOS_IL2CPP_INT32 ChaosMarshalGetLastPInvokeError(void) noexcept
{
    auto* ts = GetCurrentThreadState();
    if (ts == nullptr) return 0;
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
    if (ts == nullptr) return 0;
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

// ── String creation helper ──────────────────────────────────
// Forward declaration: StringNewUtf8 is defined in object_creation.cpp
// (Unity build), linked through chaos_codegen.lib.
void* StringNewUtf8(RuntimeState* runtime_state, ThreadState* thread_state,
    const char* utf8_bytes, CHAOS_IL2CPP_UINTPTR byte_count);

CHAOS_IL2CPP_INTPTR ChaosStringCreateFromUtf8(const char* utf8, CHAOS_IL2CPP_INT32 length) noexcept
{
    if (utf8 == nullptr || length <= 0) return 0;
    auto* ts = GetCurrentThreadState();
    auto* rs = GetCurrentRuntimeState();
    if (ts == nullptr || rs == nullptr) return 0;
    auto* str = StringNewUtf8(rs, ts, utf8, static_cast<CHAOS_IL2CPP_UINTPTR>(length));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(str);
}

// ── Precompiled JSON serialization stubs ────────────────────
// Format primitive values directly as JSON strings, bypassing the managed
// JsonSerializer's runtime JsonTypeInfo<T> building and reflection chain.

CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(value));
    return ChaosStringCreateFromUtf8(buffer, len);
}

CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt64(CHAOS_IL2CPP_INT64 value) noexcept
{
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%lld", static_cast<long long>(value));
    return ChaosStringCreateFromUtf8(buffer, len);
}

CHAOS_IL2CPP_INTPTR ChaosJsonSerializeBool(CHAOS_IL2CPP_INT32 value) noexcept
{
    auto str = value ? "true" : "false";
    return ChaosStringCreateFromUtf8(str, value ? 4 : 5);
}

CHAOS_IL2CPP_INTPTR ChaosJsonSerializeString(CHAOS_IL2CPP_INTPTR value) noexcept
{
    // For string values, JsonSerializer wraps them in quotes and escapes.
    // Return the string as-is wrapped in quotes — simple JSON string value.
    // The managed String content needs to be extracted from the String object.
    // For now, return the input unchanged (caller handles JSON formatting).
    return value;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
