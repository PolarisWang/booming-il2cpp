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
// Fallback: StringNewUtf8 is defined in object_creation.cpp (runtime-core).
// This definition provides a safe stub when the prebuilt chaos_runtime_core.lib
// hasn't been rebuilt — the real implementation in object_creation.cpp
// allocates via GcAllocateAtomic and copies the UTF-8 bytes.
void* StringNewUtf8(RuntimeState* runtime_state, ThreadState* thread_state,
    const char* utf8_bytes, CHAOS_IL2CPP_UINTPTR byte_count)
{
    (void)runtime_state; (void)thread_state;
    if (utf8_bytes == nullptr || byte_count == 0u) return nullptr;
    // Minimal implementation: allocate via GcAllocateAtomic and copy.
    // This mirrors object_creation.cpp::StringNewUtf8 logic without
    // depending on IsAttached / StringObjectHeader from other TUs.
    struct StrHdr { void* type; CHAOS_IL2CPP_UINTPTR bc; };
    const CHAOS_IL2CPP_SIZE alloc_size = sizeof(StrHdr) + static_cast<CHAOS_IL2CPP_SIZE>(byte_count) + 1u;
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(alloc_size));
    if (storage == nullptr) return nullptr;
    auto* hdr = reinterpret_cast<StrHdr*>(storage);
    hdr->type = nullptr;
    hdr->bc = byte_count;
    auto* text = reinterpret_cast<char*>(storage + sizeof(StrHdr));
    if (byte_count != 0u) std::memcpy(text, utf8_bytes, static_cast<CHAOS_IL2CPP_SIZE>(byte_count));
    text[byte_count] = '\0';
    return hdr;
}

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

// ── Precompiled JSON deserialization stubs ──────────────────
// Parse JSON strings back to primitive values using atoi/strtol.

CHAOS_IL2CPP_INT32 ChaosJsonDeserializeInt32(CHAOS_IL2CPP_INTPTR jsonStr) noexcept
{
    if (jsonStr == 0) return 0;
    const char* data = stub_string_data(reinterpret_cast<void*>(jsonStr));
    if (data == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(std::atol(data));
}

CHAOS_IL2CPP_INT64 ChaosJsonDeserializeInt64(CHAOS_IL2CPP_INTPTR jsonStr) noexcept
{
    if (jsonStr == 0) return 0;
    const char* data = stub_string_data(reinterpret_cast<void*>(jsonStr));
    if (data == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INT64>(std::atoll(data));
}

CHAOS_IL2CPP_INT32 ChaosJsonDeserializeBool(CHAOS_IL2CPP_INTPTR jsonStr) noexcept
{
    if (jsonStr == 0) return 0;
    const char* data = stub_string_data(reinterpret_cast<void*>(jsonStr));
    if (data == nullptr) return 0;
    return (data[0] == 't' || data[0] == '1') ? 1 : 0;
}

// ═══════════════════════════════════════════════════════════════
// AnsiStringMarshaller stubs
// ═══════════════════════════════════════════════════════════════
// Uses engine_binding.h functions (already included).
// These are compiled from source in the test project's SDK.

CHAOS_IL2CPP_INTPTR ChaosAnsiStringMarshallerConvertToUnmanaged(CHAOS_IL2CPP_INTPTR str) noexcept
{
    auto* rs = GetCurrentRuntimeState();
    auto* ts = GetCurrentThreadState();
    if (rs == nullptr || ts == nullptr) return 0;
    return MarshalStringToCoTaskMemUtf8(rs, ts, reinterpret_cast<void*>(str));
}

CHAOS_IL2CPP_INTPTR ChaosAnsiStringMarshallerConvertToManaged(CHAOS_IL2CPP_INTPTR native) noexcept
{
    auto* rs = GetCurrentRuntimeState();
    auto* ts = GetCurrentThreadState();
    if (rs == nullptr || ts == nullptr) return 0;
    auto* result = MarshalPtrToStringUtf8(rs, ts, native, -1, false);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

void ChaosAnsiStringMarshallerFree(CHAOS_IL2CPP_INTPTR native) noexcept
{
    auto* rs = GetCurrentRuntimeState();
    if (rs == nullptr) return;
    MarshalFreeCoTaskMem(rs, native);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
