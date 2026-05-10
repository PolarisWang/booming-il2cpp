#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Runtime API declarations exposed to generated code ──
//
// This header provides declarations used by generated .cpp files via
// runtime_core.h. After the A4-Dual+V2 migration, generated code uses
// FatHeader (16B: type_info + vtable) for all object headers.

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "arithmetic_chaos_bridge.h"
#include "codegen_bridge.h"       // HotpatchEntryV0, Hotpatch structs, kHotpatchActive
#include <gc.h>                   // GC_END_STUBBORN_CHANGE for write barriers
#include "debug_sink.h"           // DebugEventSink ring buffer

// ═══════════════════════════════════════════════════════════════════
// A5-Trinity Object Header Architecture
// ═══════════════════════════════════════════════════════════════════
// Three header variants discriminated by TypeInfo.flags[1:0]:
//
//   PureType (00):  8B  {TypeInfo* type_info}
//                       — no sync, no vtable
//   ThinLockable (01): 16B {TypeInfo* type_info, uint64_t sync_state}
//                       — thin-lock capable, no vtable
//   Fat (10):          24B {TypeInfo* type_info, void** vtable, uint64_t sync_state}
//                       — full dispatch + sync
//
// All three store TypeInfo* at offset [0], so chaos_object_get_type_info()
// is a single *(TypeInfoHot**)obj read — no bit magic needed.
//
// PureType: value-type boxes, sealed types with 0 virtual methods,
//           compiler-verified no-sync.
// ThinLockable: most reference types (no virtual dispatch, can sync).
// Fat: types with virtual methods (vtable dispatch + sync).
// ═══════════════════════════════════════════════════════════════════

// ── PureType header (8B) ──────────────────────────────────────────
// Used for value-type boxes, sealed types with no virtual methods.
// No sync_state — compiler-verified no-sync.
struct PureTypeHeader {
    const TypeInfoHot* type_info = nullptr;
};

// ── ThinLockable header (16B) ─────────────────────────────────────
// Used for most reference types. sync_state at [8] for thin locking.
struct ThinLockableHeader {
    const TypeInfoHot* type_info   = nullptr;  // [0]
    uint64_t        sync_state  = 0;        // [8] — thin lock / sync block index
};

// ── Fat header (24B) ──────────────────────────────────────────────
// Full-featured: virtual dispatch table + type identity + sync.
struct FatHeader {
    const TypeInfoHot* type_info   = nullptr;  // [0]
    const void**    vtable      = nullptr;  // [8]
    uint64_t        sync_state  = 0;        // [16] — thin lock / sync block index
};

// ── Runtime ObjectHeader (24B) ────────────────────────────────────
// Runtime-internal full layout. Matches FatHeader fields at [0..15]
// so reinterpret_cast between them is safe.
// struct RuntimeObjectHeader {
//     FatHeader   header;         // 16B {type_info, vtable}
//     uint64_t    sync_state = 0; // 8B  — thin lock / sync block index
// };  // 24B (defined in runtime_core.cpp)

// ── Unified type_info accessor ─────────────────────────────────────
// All three header kinds store TypeInfoHot* at offset [0].
inline const TypeInfoHot* chaos_object_get_type_info(const void* obj) noexcept {
    return *static_cast<const TypeInfoHot* const*>(obj);
}

// ── Managed string type ──────────────────────────────────────────
// Used by generated code for reinterpret_cast access to string length.
struct chaos_managed_string {
    ThinLockableHeader header{};
    CHAOS_IL2CPP_INT32 length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;
};

// ── Managed exception type ───────────────────────────────────────
// Used by generated code for catch(chaos_managed_exception&) blocks
// and throw chaos_managed_exception{obj} statements.
struct chaos_managed_exception
{
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

#define CHAOS_IL2CPP_STRING_TYPE chaos_managed_string

// ── Exception metadata functions (extern C declarations) ──
// Implementations live in exception_api.cpp.
namespace chaos::il2cpp::runtime_core {
extern "C" {
void ChaosReflectionSetExceptionMetadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Runtime stub declarations ───────────────────────────────────
// Stub implementations in runtime_stubs.cpp. These are thin wrappers
// called by generated extern "C" external-runtime-helper functions.
//
// NOTE: Functions already declared in reflection_api.h (included via
// runtime_core.h) are NOT duplicated here to avoid redefinition errors.
extern "C" {
// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;
// Type marshalling
CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept;
// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;
// DateTime
CHAOS_IL2CPP_INT64  ChaosDatetimeGetUtcNow(void) noexcept;
CHAOS_IL2CPP_INT32  ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept;
// Math
CHAOS_IL2CPP_INT64  ChaosMathSqrt(CHAOS_IL2CPP_INT64 value) noexcept;
// Threading
void    ChaosInterlockedMemoryBarrier(void) noexcept;
// Exception
CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept;
// Object
CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
void    ChaosObjectCtor(CHAOS_IL2CPP_INTPTR obj) noexcept;
// GUID / Random / HashCode
CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept;
CHAOS_IL2CPP_INT32  ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidToString(CHAOS_IL2CPP_INTPTR guid) noexcept;
void    ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept;
CHAOS_IL2CPP_INT64  ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept;
CHAOS_IL2CPP_INT32  ChaosHashCodeToHashCode(CHAOS_IL2CPP_INTPTR state) noexcept;
CHAOS_IL2CPP_INT32  ChaosHashCodeCombine2(CHAOS_IL2CPP_INT32 hc1, CHAOS_IL2CPP_INT32 hc2) noexcept;
void    ChaosHashCodeAdd(CHAOS_IL2CPP_INTPTR state, CHAOS_IL2CPP_INT32 value) noexcept;
	// Convert / Parse / Format
	CHAOS_IL2CPP_INT32  ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_UINT8  ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT16  ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT32  ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT64  ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT32  ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
	CHAOS_IL2CPP_INT32  ChaosParseInt32(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT64  ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept;
	CHAOS_IL2CPP_INT32  ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
// String
CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringEndsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringToLower(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringToUpper(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringTrim(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringReplace(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR old_value, CHAOS_IL2CPP_INTPTR new_value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringSubstring(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringRemove(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFormat(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0, CHAOS_IL2CPP_INTPTR arg1) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFastAllocate(CHAOS_IL2CPP_INTPTR length) noexcept;
void    ChaosStringAppend(CHAOS_IL2CPP_INTPTR builder, CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR arg) noexcept;
// GC
void    ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetMaxGeneration(void) noexcept;
// RuntimeType
CHAOS_IL2CPP_INTPTR ChaosRuntimeTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept;
// Type
CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTypeEquals(CHAOS_IL2CPP_INTPTR type_a, CHAOS_IL2CPP_INTPTR type_b) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeInfo(CHAOS_IL2CPP_INTPTR type) noexcept;
// Interlocked
CHAOS_IL2CPP_INT64  ChaosInterlockedReadInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INT32* location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept;
void    ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept;
// Environment
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept;
// Console
void    ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept;
// Delegate
void    ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept;

// ── Thread ─────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept;
// ── Volatile ──────────────────────────────────────────────────
CHAOS_IL2CPP_INT32  ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept;

// ══════════════════════════════════════════════════════════════
// NOTE: Functions declared in reflection_api.h (included via
// runtime_core.h) are NOT redeclared here. Only additional stubs
// not present in reflection_api.h are declared below.
// ══════════════════════════════════════════════════════════════

// ── Pure stubs (only in runtime_stubs.cpp, NOT in reflection_api.h/cpp) ──
CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
CHAOS_IL2CPP_INT32  ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept;
}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_