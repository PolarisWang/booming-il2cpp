// ── Interop stub declarations ──────────────────────────────────
// Simple interop stubs for Marshal/COM methods that don't need
// prebuilt library linkage. Compiled from source via TPG.
#pragma once

#include <chaos/native_types.h>
#include <cstdint>

// Marshal P/Invoke error helpers
void    ChaosMarshalSetLastPInvokeError(CHAOS_IL2CPP_INT32 error) noexcept;
CHAOS_IL2CPP_INT32 ChaosMarshalGetLastPInvokeError(void) noexcept;

// Exception code/pointers stubs
CHAOS_IL2CPP_INT32 ChaosMarshalGetExceptionCode(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionPointers(void) noexcept;

// GetHRForLastWin32Error: converts last P/Invoke error to HRESULT
CHAOS_IL2CPP_INT32 ChaosMarshalGetHRForLastWin32Error(void) noexcept;

// COM cleanup check stub
CHAOS_IL2CPP_INT32 ChaosMarshalAreComObjectsAvailableForCleanup(void) noexcept;

// ── JSON string creation helper ──────────────────────────────
// Creates a managed String from UTF-8 bytes. Used by precompiled JSON
// serializer stubs to return formatted JSON strings without going through
// managed JsonSerializer's reflection path.
CHAOS_IL2CPP_INTPTR ChaosStringCreateFromUtf8(const char* utf8, CHAOS_IL2CPP_INT32 length) noexcept;

// ── Precompiled JSON serialization stubs ─────────────────────
// Bypass managed JsonSerializer for primitive types by formatting values
// directly as JSON strings using snprintf, then wrapping the result as a
// managed String via ChaosStringCreateFromUtf8.
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeString(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeBool(CHAOS_IL2CPP_INT32 value) noexcept;

// ── Precompiled JSON deserialization stubs ───────────────────
// Parse JSON strings back to primitive values using atoi/strtol.
CHAOS_IL2CPP_INT32 ChaosJsonDeserializeInt32(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;
CHAOS_IL2CPP_INT64 ChaosJsonDeserializeInt64(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;
CHAOS_IL2CPP_INT32 ChaosJsonDeserializeBool(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;

// ── AnsiStringMarshaller stubs ───────────────────────────────
// Source-generated marshalling for [DllImport] with CharSet.Ansi.
// Implementations delegate to the runtime's CoTaskMem UTF-8 helpers.
CHAOS_IL2CPP_INTPTR ChaosAnsiStringMarshallerConvertToUnmanaged(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAnsiStringMarshallerConvertToManaged(CHAOS_IL2CPP_INTPTR native) noexcept;
void ChaosAnsiStringMarshallerFree(CHAOS_IL2CPP_INTPTR native) noexcept;

// ── GCHandle stubs ───────────────────────────────────────────
// DirectNativeSymbol stubs for GCHandle.Alloc/Free/GetTarget.
// Implementations delegate to the GC handle API (GcCreateStrongHandle, etc.).
CHAOS_IL2CPP_INTPTR ChaosGCHandleAlloc(CHAOS_IL2CPP_INTPTR obj) noexcept;
void   ChaosGCHandleFree(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGCHandleGetTarget(CHAOS_IL2CPP_INTPTR handle) noexcept;

// ── Marshal HR stub declarations ─────────────────────────────
// DirectNativeSymbol stubs for Marshal.GetExceptionForHR,
// ThrowExceptionForHR, GetHRForException.  Implementations
// delegate to engine_binding.h functions.
CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionForHR(CHAOS_IL2CPP_INT32 errorCode) noexcept;
void   ChaosMarshalThrowExceptionForHR(CHAOS_IL2CPP_INT32 errorCode) noexcept;
CHAOS_IL2CPP_INT32 ChaosMarshalGetHRForException(CHAOS_IL2CPP_INTPTR exceptionObj) noexcept;

// ── NativeMemory stubs ──────────────────────────────────────
// DirectNativeSymbol stubs for NativeMemory.Alloc/Free/Realloc/AlignedAlloc.
CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlloc(CHAOS_IL2CPP_INTPTR byteCount) noexcept;
void   ChaosNativeMemoryFree(CHAOS_IL2CPP_INTPTR ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosNativeMemoryRealloc(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INTPTR byteCount) noexcept;
CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlignedAlloc(CHAOS_IL2CPP_INTPTR byteCount, CHAOS_IL2CPP_INTPTR alignment) noexcept;
void   ChaosNativeMemoryAlignedFree(CHAOS_IL2CPP_INTPTR ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosNativeMemoryAlignedRealloc(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INTPTR byteCount, CHAOS_IL2CPP_INTPTR alignment) noexcept;

// ── SafeBuffer stubs ────────────────────────────────────────
// DirectNativeSymbol stubs for SafeBuffer.GetByteLength/ReadByte/WriteByte.
CHAOS_IL2CPP_INT32 ChaosSafeBufferGetByteLength(CHAOS_IL2CPP_INTPTR safeBuffer) noexcept;
CHAOS_IL2CPP_UINT8 ChaosSafeBufferReadByte(CHAOS_IL2CPP_INTPTR safeBuffer, CHAOS_IL2CPP_INTPTR position) noexcept;
void   ChaosSafeBufferWriteByte(CHAOS_IL2CPP_INTPTR safeBuffer, CHAOS_IL2CPP_INTPTR position, CHAOS_IL2CPP_UINT8 value) noexcept;

// ── ComWrappers stubs ────────────────────────────────────────
// Stubs for ComWrappers static methods (AOT — COM not available).
CHAOS_IL2CPP_INTPTR ChaosComWrappersRegisterForMarshalling(CHAOS_IL2CPP_INTPTR wrapperObj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosComWrappersGetOrCreateComInterfaceForObject(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 flags, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosComWrappersGetOrCreateObjectForComInstance(CHAOS_IL2CPP_INTPTR comPtr, CHAOS_IL2CPP_INT32 flags, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept;
CHAOS_IL2CPP_INT32 ChaosComWrappersTryGetComInstance(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept;
CHAOS_IL2CPP_INT32 ChaosComWrappersTryGetObject(CHAOS_IL2CPP_INTPTR comPtr, CHAOS_IL2CPP_INTPTR wrapperObj) noexcept;

// ── NativeLibrary stubs ─────────────────────────────────────
// DirectNativeSymbol stubs for NativeLibrary.Load/Free/GetExport.
CHAOS_IL2CPP_INTPTR ChaosNativeLibraryLoad(CHAOS_IL2CPP_INTPTR nameObj) noexcept;
CHAOS_IL2CPP_INT32 ChaosNativeLibraryFree(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosNativeLibraryGetExport(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR nameObj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosNativeLibraryGetMainProgramHandle(void) noexcept;

// ── External runtime fallback stub ──────────────────────────
// Returns type-appropriate default values (0/nullptr) for
// unresolved external runtime method calls. Called from generated
// dispatch code when kChaosExternalRuntimeFnTable[idx] is null.
// This prevents CHAOS_IL2CPP_FAIL/crash for methods without
// DirectNativeSymbol stubs or runtime DLL resolution.
CHAOS_IL2CPP_INTPTR ChaosExternalRuntimeFallback(const char* subject_id) noexcept;

// ── External runtime IL data entry ─────────────────────────
// Each entry carries raw CIL bytes + optional AotCoreIr JSON for
// interpreter fallback when hotpatch dispatch is unavailable.
// Defined in generated code and used by ChaosExternalRuntimeFallback.
struct ChaosIlDataEntry {
    const char*         subject_id;
    const uint8_t*      il_data;
    int32_t             il_size;
    void*               patch_method;
    const char*         json_data;
};

// Generated data table: embedded CIL + JSON for interpreter fallback.
// Terminated by a sentinel entry with subject_id == nullptr.
extern "C" ChaosIlDataEntry kChaosExternalRuntimeIlData[];
