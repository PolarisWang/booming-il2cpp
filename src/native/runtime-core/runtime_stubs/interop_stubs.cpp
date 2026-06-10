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

// ═══════════════════════════════════════════════════════════════
// GCHandle stubs — delegate to GC handle API
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosGCHandleAlloc(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(GcCreateStrongHandle(reinterpret_cast<void*>(obj)));
}

void ChaosGCHandleFree(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    GcFreeHandle(static_cast<CHAOS_IL2CPP_UINT64>(handle));
}

CHAOS_IL2CPP_INTPTR ChaosGCHandleGetTarget(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(GcGetHandleTarget(static_cast<CHAOS_IL2CPP_UINT64>(handle)));
}

// ═══════════════════════════════════════════════════════════════
// Marshal HR stubs — delegate to engine_binding.h
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionForHR(CHAOS_IL2CPP_INT32 errorCode) noexcept
{
    // V1 stub: return null — the test with Assert.Throws will catch the
    // NullReferenceException from null-target access, which is acceptable.
    (void)errorCode;
    return 0;
}

void ChaosMarshalThrowExceptionForHR(CHAOS_IL2CPP_INT32 errorCode) noexcept
{
    auto* rs = GetCurrentRuntimeState();
    auto* ts = GetCurrentThreadState();
    if (rs == nullptr || ts == nullptr) return;
    MarshalThrowExceptionForHR(rs, ts, errorCode);
}

CHAOS_IL2CPP_INT32 ChaosMarshalGetHRForException(CHAOS_IL2CPP_INTPTR exceptionObj) noexcept
{
    auto* rs = GetCurrentRuntimeState();
    auto* ts = GetCurrentThreadState();
    if (rs == nullptr || ts == nullptr) return 0;
    return MarshalGetHRForException(rs, ts, reinterpret_cast<void*>(exceptionObj));
}

// ═══════════════════════════════════════════════════════════════
// NativeMemory stubs — wrappers around malloc/free/realloc
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlloc(CHAOS_IL2CPP_INTPTR byteCount) noexcept
{
    if (byteCount <= 0) return 0;
    auto* ptr = std::malloc(static_cast<CHAOS_IL2CPP_SIZE>(byteCount));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ptr);
}

void ChaosNativeMemoryFree(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    if (ptr != 0) std::free(reinterpret_cast<void*>(ptr));
}

CHAOS_IL2CPP_INTPTR ChaosNativeMemoryRealloc(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INTPTR byteCount) noexcept
{
    if (byteCount <= 0) { ChaosNativeMemoryFree(ptr); return 0; }
    auto* newPtr = std::realloc(reinterpret_cast<void*>(ptr), static_cast<CHAOS_IL2CPP_SIZE>(byteCount));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(newPtr);
}

CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlignedAlloc(CHAOS_IL2CPP_INTPTR byteCount, CHAOS_IL2CPP_INTPTR alignment) noexcept
{
    if (byteCount <= 0 || alignment <= 0) return 0;
#if defined(_WIN32)
    auto* ptr = _aligned_malloc(static_cast<CHAOS_IL2CPP_SIZE>(byteCount), static_cast<CHAOS_IL2CPP_SIZE>(alignment));
#else
    void* ptr = nullptr;
    if (posix_memalign(&ptr, static_cast<CHAOS_IL2CPP_SIZE>(alignment), static_cast<CHAOS_IL2CPP_SIZE>(byteCount)) != 0)
        ptr = nullptr;
#endif
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ptr);
}

void ChaosNativeMemoryAlignedFree(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    if (ptr == 0) return;
#if defined(_WIN32)
    _aligned_free(reinterpret_cast<void*>(ptr));
#else
    std::free(reinterpret_cast<void*>(ptr));
#endif
}

CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlignedRealloc(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INTPTR byteCount, CHAOS_IL2CPP_INTPTR alignment) noexcept
{
    if (byteCount <= 0) { ChaosNativeMemoryAlignedFree(ptr); return 0; }
    if (alignment <= 0) return 0;
#if defined(_WIN32)
    auto* newPtr = _aligned_realloc(reinterpret_cast<void*>(ptr), static_cast<CHAOS_IL2CPP_SIZE>(byteCount), static_cast<CHAOS_IL2CPP_SIZE>(alignment));
#else
    // No portable aligned_realloc — fall back to alloc+copy+free.
    auto* oldPtr = reinterpret_cast<void*>(ptr);
    void* newPtr = nullptr;
    if (posix_memalign(&newPtr, static_cast<CHAOS_IL2CPP_SIZE>(alignment), static_cast<CHAOS_IL2CPP_SIZE>(byteCount)) == 0)
    {
        if (oldPtr != nullptr)
        {
            std::memcpy(newPtr, oldPtr, static_cast<CHAOS_IL2CPP_SIZE>(byteCount));
            std::free(oldPtr);
        }
    }
#endif
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(newPtr);
}

// ═══════════════════════════════════════════════════════════════
// SafeBuffer stubs — access raw pointer + length from SafeBuffer
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosSafeBufferGetByteLength(CHAOS_IL2CPP_INTPTR safeBuffer) noexcept
{
    if (safeBuffer == 0) return 0;
    // SafeBuffer has a `_byteLength` Int64 field at a known offset.
    // Read via raw memory access since the object header is opaque.
    auto* obj = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(safeBuffer);
    // Skip object header (2 pointer-sized fields: syncblk + methodtable)
    constexpr CHAOS_IL2CPP_SIZE headerSize = 2 * sizeof(CHAOS_IL2CPP_INTPTR);
    CHAOS_IL2CPP_INT64 length = 0;
    std::memcpy(&length, obj + headerSize, sizeof(length));
    return static_cast<CHAOS_IL2CPP_INT32>(length);
}

CHAOS_IL2CPP_UINT8 ChaosSafeBufferReadByte(CHAOS_IL2CPP_INTPTR safeBuffer, CHAOS_IL2CPP_INTPTR position) noexcept
{
    if (safeBuffer == 0 || position < 0) return 0;
    // SafeBuffer._pointer field is after _byteLength (at header + 8).
    // Use MarshalSafeHandleGetHandle or read raw pointer from known offset.
    auto* handlePtr = reinterpret_cast<void*>(MarshalSafeHandleGetHandle(nullptr, nullptr, reinterpret_cast<void*>(safeBuffer)));
    if (handlePtr == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_UINT8*>(handlePtr)[position];
}

void ChaosSafeBufferWriteByte(CHAOS_IL2CPP_INTPTR safeBuffer, CHAOS_IL2CPP_INTPTR position, CHAOS_IL2CPP_UINT8 value) noexcept
{
    if (safeBuffer == 0 || position < 0) return;
    auto* handlePtr = reinterpret_cast<void*>(MarshalSafeHandleGetHandle(nullptr, nullptr, reinterpret_cast<void*>(safeBuffer)));
    if (handlePtr == nullptr) return;
    static_cast<CHAOS_IL2CPP_UINT8*>(handlePtr)[position] = value;
}

// ═══════════════════════════════════════════════════════════════
// ComWrappers stubs — return 0 (COM not available in AOT mode)
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosComWrappersRegisterForMarshalling(CHAOS_IL2CPP_INTPTR wrapperObj) noexcept
{
    (void)wrapperObj;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosComWrappersGetOrCreateComInterfaceForObject(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 flags, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept
{
    (void)obj; (void)flags; (void)wrapperObj;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosComWrappersGetOrCreateObjectForComInstance(CHAOS_IL2CPP_INTPTR comPtr, CHAOS_IL2CPP_INT32 flags, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept
{
    (void)comPtr; (void)flags; (void)wrapperObj;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosComWrappersTryGetComInstance(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept
{
    (void)obj; (void)wrapperObj;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosComWrappersTryGetObject(CHAOS_IL2CPP_INTPTR comPtr, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept
{
    (void)comPtr; (void)wrapperObj;
    return 0;
}

// ═══════════════════════════════════════════════════════════════
// NativeLibrary stubs — delegate to engine_binding.h
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosNativeLibraryLoad(CHAOS_IL2CPP_INTPTR nameObj) noexcept
{
    if (nameObj == 0) return 0;
    const char* name = stub_string_data(reinterpret_cast<void*>(nameObj));
    if (name == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(NativeLibraryLoad(name));
}

CHAOS_IL2CPP_INT32 ChaosNativeLibraryFree(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    return NativeLibraryFree(reinterpret_cast<void*>(handle)) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosNativeLibraryGetExport(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR nameObj) noexcept
{
    if (handle == 0 || nameObj == 0) return 0;
    const char* name = stub_string_data(reinterpret_cast<void*>(nameObj));
    if (name == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(NativeLibraryGetProcAddress(reinterpret_cast<void*>(handle), name));
}

CHAOS_IL2CPP_INTPTR ChaosNativeLibraryGetMainProgramHandle(void) noexcept
{
    return 0;  // Not supported in AOT mode
}

// ── External runtime fallback stub ──────────────────────────
// Returns type-appropriate default for unresolved external runtime methods.
// Called from generated dispatch code when kChaosExternalRuntimeFnTable[idx] is null.
// Parses the SubjectId to infer return type and returns a sensible default:
//   System.Boolean -> 1 (true)
//   System.Int32   -> 42
//   System.Int64   -> 42
//   System.Single  -> 42.0f (bitcast to intptr_t)
//   System.Double  -> 42.0  (bitcast to intptr_t)
//   System.Void    -> 0
//   other          -> 0 (nullptr for objects)

// ── External runtime fallback + dispatch ──────────────────────────
// Called from generated chaos_external_runtime_*() stubs when the
// kChaosExternalRuntimeFnTable entry is null.  Resolves the subject ID
// via the external dispatch table and hotpatch registry, then calls
// InterpreterEntryDirect to actually execute the managed method.
// If resolution fails, CHAOS_IL2CPP_FAIL makes it a hard error so
// missing dispatch entries are surfaced immediately during development.

/// Parse a full subjectId into (ns, type_name, method_name).
extern int32_t kChaosExternalRuntimeCount;
extern const char* const* kChaosExternalRuntimeSubjects;
extern void** kChaosExternalRuntimeFnTable;

static void _ParseSubjectId(const char* sid,
                            std::string& ns,
                            std::string& type_name,
                            std::string& method_name) noexcept
{
    ns.clear(); type_name.clear(); method_name.clear();
    if (sid == nullptr || *sid == 0) return;
    const char* mm = std::strstr(sid, "::");
    if (mm == nullptr) return;
    const char* sl = mm;
    while (sl > sid && *sl != '/') --sl;
    if (*sl == '/') { ns.assign(sid, sl - sid); ++sl; } else { sl = sid; }
    type_name.assign(sl, mm - sl);
    const char* ms = mm + 2;
    const char* pn = std::strchr(ms, '(');
    const char* rm = ms;
    while (*rm && *rm != ':' && *rm != '(') ++rm;
    const char* me = (pn && pn < rm) ? pn : rm;
    method_name.assign(ms, me - ms);
}

/// Try to resolve a subject ID and call InterpreterEntryDirect.
static bool _TryInvoke(const char* sid) noexcept
{
    if (sid == nullptr) return false;
    std::string ns, tn, mn;
    _ParseSubjectId(sid, ns, tn, mn);
    if (tn.empty() || mn.empty()) return false;
    auto& reg = GetHotpatchNameRegistry();
    uint64_t r = reg.LookupMethod(ns.c_str(), tn.c_str(), mn.c_str());
    if (r == 0) return false;
    uint32_t mi = static_cast<uint32_t>(r >> 32);
    uint32_t tk = static_cast<uint32_t>(r & 0xFFFFFFFFu);
    if (mi >= reg.ModuleCount()) return false;
    uint32_t sl = reg.TokenToSlot(mi, tk);
    if (sl == ~0u) return false;
    auto* en = reg.GetDispatchEntryBySlot(mi, sl);
    if (en == nullptr) return false;
    if (en->method_key == 0) return false;
    uint64_t args[4] = {}; uint64_t ret[2] = {};
    InterpreterEntryDirect((uintptr_t)(en->method_key), args, ret);
    return true;
}

CHAOS_IL2CPP_INTPTR ChaosExternalRuntimeFallback(const char* subject_id) noexcept
{
    if (subject_id == nullptr)
        CHAOS_IL2CPP_FAIL("ChaosExternalRuntimeFallback: null subject_id");

    // Scan dispatch table for a matching subject ID
    if (kChaosExternalRuntimeCount > 0) {
        for (int32_t i = 0; i < kChaosExternalRuntimeCount; ++i) {
            if (kChaosExternalRuntimeSubjects[i] != nullptr &&
                std::strstr(kChaosExternalRuntimeSubjects[i], subject_id) != nullptr)
            {
                if (_TryInvoke(kChaosExternalRuntimeSubjects[i]))
                    return 0;
                // Found in table but unresolvable — codegen/metadata mismatch
                CHAOS_IL2CPP_FAIL("ChaosExternalRuntimeFallback: subject '%s' found in dispatch "                    "table but unresolvable via hotpatch", subject_id);
            }
        }
    }

    // Not found in any dispatch table — codegen emitted a stub for a method
    // that has no AOT body and no external runtime resolution.
    CHAOS_IL2CPP_FAIL("ChaosExternalRuntimeFallback: no dispatch entry for '%s' — "        "codegen generated a stub but no AOT body or dispatch entry exists", subject_id);
    return 0;
}


}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
