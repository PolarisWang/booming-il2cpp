#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include <runtime_abi.h>
#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

// ── Finally scope guard (RAII for generated code try/finally in CPP_THROW mode) ──
#include <chaos/finally.h>

// ── Managed exception helpers for generated code ──
// ResolveTypeByName, RaiseManagedException, and convenience wrappers.
// Included early (before stubs.h) so inline stubs like
// ChaosConvertToInt32FromDouble can use RaiseManagedException directly.
#include "exception_helpers.h"

// ── Thread state declarations ──
// Included early (before generated_code_compat.h → stubs.h → threading_stubs.h)
// so thread_state.h is NOT first seen inside stubs.h's `extern "C"` block.
// If thread_state.h is first included from within an extern "C" context, ALL
// its declarations (RegisterThread, tls_this_thread, SafepointPoll, etc.)
// acquire C linkage — producing mangled-linker-symbol mismatches with the
// C++ definitions in thread_state.cpp.
#include "thread_state.h"

// ── Compatibility declarations for older generated .cpp files ──
// Must be included early (before any namespace) so declarations are at
// global scope and visible inside anonymous namespaces in generated files.
#include "generated_code_compat.h"

// ── Exception delivery (dual-path: setjmp/longjmp or C++ throw) ───────
#include "exception_jmp.h"

// ── Type-only sub-headers (each declares types in its own namespace) ──
#include "marshal_abi.h"
#include "com_abi.h"
#include "com_ccw.h"
#include "runtime_capability.h"
#include "convert.h"
#include "numerics_carriers.h"
#include "string_table.h"
#include "method_table.h"
#include "gc_transition.h"

// ── Phase 1: Declare types needed by sub-headers below ──
#include "reflection_api.h"

// ── Hotpatch dispatch: InterpreterEntryDirect for patched-method dispatch ──
// Generated code (codegen-emitted module registration) references
// chaos::il2cpp::runtime_core::InterpreterEntryDirect in dispatch table
// entries.  Include the declaration here so it is visible to all generated
// code that includes runtime_core.h.
#include "interpreter_entry.h"

// ── Static variable indirect access for hot-update assemblies ──
// AOT codegen emits static_var_store_read/write_field calls for hot-update
// assembly static fields instead of direct global variable access.
#include "static_var_store.h"

// Engine lifecycle declarations (extern globals, handle table, GC handle state)
// Must precede core/*.cpp inclusion so all unity sub-files see the symbols.
#include "core/engine_lifecycle.h"
#include "core/delegate_helpers.h"

namespace chaos::il2cpp::runtime_core {

// Bring types from other namespaces into runtime_core for convenient use.
// Note: explicit using-declarations only -- no blanket using-namespace directives.
using chaos::il2cpp::marshal_abi::MarshalPlatformAbiRootV1;
using chaos::il2cpp::marshal_abi::TaskRuntimeKernelV1;
using chaos::il2cpp::runtime_capability::ValueTypeKernelBackendKind;
using chaos::il2cpp::runtime_capability::VectorKernelBackendKind;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsVector2Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsVector3Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsVector4Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsMatrix3x2Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsMatrix4x4Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeNumericsQuaternionCarrier;
using chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector64Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;
using chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector512Carrier;

/// Thread-local RuntimeState for code paths that do not carry
/// an explicit RuntimeState* parameter (e.g. NativeAot external helpers).
void SetCurrentRuntimeState(RuntimeState* runtime_state);
RuntimeState* GetCurrentRuntimeState();

void SetCurrentThreadState(ThreadState* thread_state);
ThreadState* GetCurrentThreadState();

// Thread lifecycle — declared here for cross-TU visibility (used by threading_stubs.cpp).
RuntimeStatus CHAOS_RUNTIME_ABI_CALL ThreadAttach(
    RuntimeState* runtime_state,
    ThreadState** out_thread_state);
void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state);


using FinalizerCallback = void (*)(void* object_instance);

enum class RuntimeMode {
    Aot = 0,
    Mixed = 1,
};

const RuntimeAbiV0* GetRuntimeAbiV0();
const MarshalPlatformAbiRootV1* GetMarshalPlatformAbiRootV1();
const TaskRuntimeKernelV1* GetTaskRuntimeKernelV1();

// ── Delegate thunk registry (P/Invoke delegate marshalling) ──
/// Called at startup (from generated code) to register a pre-compiled
/// native thunk function for a delegate type.  target_slot is a module-level
/// static variable that the thunk reads to get the current delegate instance.
/// param_count is the arity of the delegate's Invoke method (0-4).
void RegisterDelegateThunk(const char* type_id, void* thunk_fn,
    CHAOS_IL2CPP_INTPTR* target_slot, uint8_t param_count = 0);

/// Look up a thunk function pointer by delegate type ID.
/// Returns nullptr if the type was not registered at startup.
void* FindDelegateThunk(const char* type_id);

/// Runtime helper for Marshal.GetFunctionPointerForDelegate.
/// Stores the delegate in the per-type target slot and returns the thunk address.
/// Returns nullptr if the delegate type has no registered thunk.
void* MarshalGetFunctionPointerForDelegateImpl(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR delegate_obj,
    const char* delegate_type_id);

/// Runtime helper for Marshal.GetDelegateForFunctionPointer.
/// Creates a wrapper delegate that forwards to the native function pointer.
/// The native function is invoked through an arity-aware dispatch thunk
/// that strips the managed calling convention (RuntimeState*, ThreadState*)
/// and forwards pointer-sized arguments directly.
/// V1 limitation: all args treated as CHAOS_IL2CPP_INTPTR; no CallingConvention.
void* MarshalGetDelegateForFunctionPointerImpl(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR native_fn_ptr,
    const char* delegate_type_id);

// ── Struct marshalling descriptor registry ──
/// Register a codegen-emitted static StructMarshallingDescriptorV1 for a type.
void RegisterStaticMarshallingDescriptor(
    CHAOS_IL2CPP_UINT64 stable_id,
    const marshal_abi::StructMarshallingDescriptorV1* desc) noexcept;

/// Register the field-names array for a type identified by its stable_id.
/// Called at startup alongside RegisterStaticMarshallingDescriptor.
/// field_names must be a static constexpr const char*[] in the same order
/// as the descriptor's fields[] array.
void RegisterStaticMarshallingFieldNames(
    CHAOS_IL2CPP_UINT64 stable_id,
    const char* const* field_names) noexcept;

/// Look up field names by stable_id.  Returns nullptr if not registered.
const char* const* ResolveStaticMarshallingFieldNames(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Look up a struct marshalling descriptor by TypeInfo*.
/// Priority: 1) static registry (codegen) → 2) runtime reflection build (cached).
const marshal_abi::StructMarshallingDescriptorV1*
ResolveStructMarshallingDescriptor(const TypeInfo* type) noexcept;

}  // namespace chaos::il2cpp::runtime_core

// ── Sub-headers providing additional runtime_core function declarations ──
// Each opens its own namespace chaos::il2cpp::runtime_core { ... } block.
#include "gc_helpers.h"
#include "gc_api.h"
#include "gc/gc_region.h"
#include "gc/gc_root_change.h"
#include "core/gc_alloc_stubs.h"
#include "thread_state.h"
#include "engine_binding.h"
#include "char_classification.h"
#include "half_classification.h"
#include "runtime_stubs/threading_stubs.h"

// ── Template/inline helpers in their own sub-namespaces ──
// These must come AFTER gc_helpers.h so that qualified names like
// chaos::il2cpp::runtime_core::GetCurrentRuntimeState() are visible at
// template definition time (MSVC two-phase lookup).
#include "vector_fixed_templates.h"
#include "math_kernel_helpers.h"
#include "marshal_copy_helpers.h"

// ── Phase 2: Remaining runtime_core declarations ──
namespace chaos::il2cpp::runtime_core {

// ── Single / Double / NFloat classification (implemented in runtime_core.cpp) ──

CHAOS_IL2CPP_INT32 SingleCompare(float left_value, float right_value);
bool SingleEquals(float left_value, float right_value);
bool SingleIsEvenInteger(float value);
bool SingleIsFinite(float value);
bool SingleIsNaN(float value);
bool SingleIsNaNOrZero(float value);
bool SingleIsNegative(float value);
bool SingleIsNegativeInfinity(float value);
bool SingleIsNormal(float value);
bool SingleIsOddInteger(float value);
bool SingleIsPositive(float value);
bool SingleIsPositiveInfinity(float value);
bool SingleIsPow2(float value);
bool SingleIsRealNumber(float value);
bool SingleIsSubnormal(float value);
bool SingleIsZero(float value);
bool SingleIsInteger(float value);
bool SingleIsInfinity(float value);

CHAOS_IL2CPP_INT32 DoubleCompare(double left_value, double right_value);
bool DoubleEquals(double left_value, double right_value);
bool DoubleIsEvenInteger(double value);
bool DoubleIsFinite(double value);
bool DoubleIsNaN(double value);
bool DoubleIsNaNOrZero(double value);
bool DoubleIsNegative(double value);
bool DoubleIsNegativeInfinity(double value);
bool DoubleIsNormal(double value);
bool DoubleIsOddInteger(double value);
bool DoubleIsPositive(double value);
bool DoubleIsPositiveInfinity(double value);
bool DoubleIsPow2(double value);
bool DoubleIsRealNumber(double value);
bool DoubleIsSubnormal(double value);
bool DoubleIsZero(double value);
bool DoubleIsInteger(double value);
bool DoubleIsInfinity(double value);

bool NFloatIsEvenInteger(double value);
bool NFloatIsFinite(double value);
bool NFloatIsNaN(double value);
bool NFloatIsNegative(double value);
bool NFloatIsNegativeInfinity(double value);
bool NFloatIsNormal(double value);
bool NFloatIsOddInteger(double value);
bool NFloatIsPositive(double value);
bool NFloatIsPositiveInfinity(double value);
bool NFloatIsPow2(double value);
bool NFloatIsRealNumber(double value);
bool NFloatIsSubnormal(double value);
bool NFloatIsInteger(double value);
bool NFloatIsInfinity(double value);
CHAOS_IL2CPP_INT32 NFloatCompare(double left_value, double right_value);
bool NFloatEquals(double left_value, double right_value);

CHAOS_IL2CPP_INT32 Int128Compare(const void* left_value, const void* right_value);
bool Int128Equals(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 UInt128Compare(const void* left_value, const void* right_value);
bool UInt128Equals(const void* left_value, const void* right_value);

CHAOS_IL2CPP_INT32 IntPtrCompare(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value);
bool IntPtrEquals(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value);
CHAOS_IL2CPP_INT32 UIntPtrCompare(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value);
bool UIntPtrEquals(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value);

CHAOS_IL2CPP_INT32 DateTimeCompareTicks(const void* left_value, const void* right_value);
bool DateTimeEqualsTicks(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 TimeSpanCompareTicks(const void* left_value, const void* right_value);
bool TimeSpanEqualsTicks(const void* left_value, const void* right_value);
CHAOS_IL2CPP_INT32 DateOnlyCompareDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value);
bool DateOnlyEqualsDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value);
CHAOS_IL2CPP_INT32 TimeOnlyCompareTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value);
bool TimeOnlyEqualsTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value);

// ── Numerics Vector operations (implemented in runtime_core.cpp) ──

RuntimeNumericsVector2Carrier Vector2Add(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Subtract(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Zero();
RuntimeNumericsVector2Carrier Vector2One();
RuntimeNumericsVector2Carrier Vector2UnitX();
RuntimeNumericsVector2Carrier Vector2UnitY();
RuntimeNumericsVector2Carrier Vector2Multiply(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2MultiplyScalar(RuntimeNumericsVector2Carrier value, float scalar);
RuntimeNumericsVector2Carrier Vector2Divide(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2DivideScalar(RuntimeNumericsVector2Carrier value, float scalar);
RuntimeNumericsVector2Carrier Vector2Abs(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Min(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Max(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Clamp(RuntimeNumericsVector2Carrier value, RuntimeNumericsVector2Carrier min_value, RuntimeNumericsVector2Carrier max_value);
RuntimeNumericsVector2Carrier Vector2SquareRoot(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Normalize(RuntimeNumericsVector2Carrier value);
RuntimeNumericsVector2Carrier Vector2Lerp(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value, float amount);
RuntimeNumericsVector2Carrier Vector2Reflect(RuntimeNumericsVector2Carrier vector, RuntimeNumericsVector2Carrier normal);
bool Vector2Equals(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2GreaterThanAll(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2GreaterThanOrEqualAll(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2LessThanAll(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2LessThanOrEqualAll(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
CHAOS_IL2CPP_INT32 Vector2GetHashCode(RuntimeNumericsVector2Carrier value);
float Vector2Length(RuntimeNumericsVector2Carrier value);
float Vector2GetElement(RuntimeNumericsVector2Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector2Carrier Vector2Negate(RuntimeNumericsVector2Carrier value);
float Vector2Dot(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
float Vector2LengthSquared(RuntimeNumericsVector2Carrier value);
float Vector2DistanceSquared(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix);
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
bool Vector2TryCopyTo(RuntimeNumericsVector2Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);

RuntimeNumericsVector3Carrier Vector3Add(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Subtract(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Zero();
RuntimeNumericsVector3Carrier Vector3One();
RuntimeNumericsVector3Carrier Vector3UnitX();
RuntimeNumericsVector3Carrier Vector3UnitY();
RuntimeNumericsVector3Carrier Vector3UnitZ();
RuntimeNumericsVector3Carrier Vector3Multiply(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3MultiplyScalar(RuntimeNumericsVector3Carrier value, float scalar);
RuntimeNumericsVector3Carrier Vector3Divide(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3DivideScalar(RuntimeNumericsVector3Carrier value, float scalar);
RuntimeNumericsVector3Carrier Vector3Abs(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Min(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Max(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Clamp(RuntimeNumericsVector3Carrier value, RuntimeNumericsVector3Carrier min_value, RuntimeNumericsVector3Carrier max_value);
RuntimeNumericsVector3Carrier Vector3SquareRoot(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Normalize(RuntimeNumericsVector3Carrier value);
RuntimeNumericsVector3Carrier Vector3Lerp(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value, float amount);
RuntimeNumericsVector3Carrier Vector3Reflect(RuntimeNumericsVector3Carrier vector, RuntimeNumericsVector3Carrier normal);
RuntimeNumericsVector3Carrier Vector3Cross(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
bool Vector3Equals(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3GreaterThanAll(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3GreaterThanOrEqualAll(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3LessThanAll(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3LessThanOrEqualAll(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
CHAOS_IL2CPP_INT32 Vector3GetHashCode(RuntimeNumericsVector3Carrier value);
float Vector3Length(RuntimeNumericsVector3Carrier value);
float Vector3GetElement(RuntimeNumericsVector3Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector3Carrier Vector3Negate(RuntimeNumericsVector3Carrier value);
float Vector3Dot(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
float Vector3LengthSquared(RuntimeNumericsVector3Carrier value);
float Vector3DistanceSquared(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value);
RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector3Carrier Vector3TransformNormal(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
bool Vector3TryCopyTo(RuntimeNumericsVector3Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);

RuntimeNumericsVector4Carrier Vector4Add(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Subtract(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Zero();
RuntimeNumericsVector4Carrier Vector4One();
RuntimeNumericsVector4Carrier Vector4UnitX();
RuntimeNumericsVector4Carrier Vector4UnitY();
RuntimeNumericsVector4Carrier Vector4UnitZ();
RuntimeNumericsVector4Carrier Vector4UnitW();
RuntimeNumericsVector4Carrier Vector4Multiply(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4MultiplyScalar(RuntimeNumericsVector4Carrier value, float scalar);
RuntimeNumericsVector4Carrier Vector4Divide(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4DivideScalar(RuntimeNumericsVector4Carrier value, float scalar);
RuntimeNumericsVector4Carrier Vector4Abs(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Min(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Max(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Clamp(RuntimeNumericsVector4Carrier value, RuntimeNumericsVector4Carrier min_value, RuntimeNumericsVector4Carrier max_value);
RuntimeNumericsVector4Carrier Vector4SquareRoot(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Normalize(RuntimeNumericsVector4Carrier value);
RuntimeNumericsVector4Carrier Vector4Lerp(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value, float amount);
bool Vector4Equals(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4GreaterThanAll(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4GreaterThanOrEqualAll(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4LessThanAll(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4LessThanOrEqualAll(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
CHAOS_IL2CPP_INT32 Vector4GetHashCode(RuntimeNumericsVector4Carrier value);
float Vector4Length(RuntimeNumericsVector4Carrier value);
float Vector4GetElement(RuntimeNumericsVector4Carrier value, CHAOS_IL2CPP_INT32 index);
RuntimeNumericsVector4Carrier Vector4Negate(RuntimeNumericsVector4Carrier value);
float Vector4Dot(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
float Vector4LengthSquared(RuntimeNumericsVector4Carrier value);
float Vector4DistanceSquared(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsMatrix4x4Carrier matrix);
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsQuaternionCarrier rotation);
bool Vector4TryCopyTo(RuntimeNumericsVector4Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index);

RuntimeIntrinsicVector64Carrier Vector64Reinterpret(RuntimeIntrinsicVector64Carrier value);
RuntimeIntrinsicVector128Carrier Vector128Reinterpret(RuntimeIntrinsicVector128Carrier value);
RuntimeIntrinsicVector256Carrier Vector256Reinterpret(RuntimeIntrinsicVector256Carrier value);
RuntimeIntrinsicVector512Carrier Vector512Reinterpret(RuntimeIntrinsicVector512Carrier value);
CHAOS_IL2CPP_INT32 Vector64Count();
CHAOS_IL2CPP_INT32 Vector128Count();
CHAOS_IL2CPP_INT32 Vector256Count();
CHAOS_IL2CPP_INT32 Vector512Count();
CHAOS_IL2CPP_INT32 VectorPlatformByteWidth();
bool Vector64IsSupported();
bool Vector128IsSupported();
bool Vector256IsSupported();
bool Vector512IsSupported();

// ── Decimal helpers ──
struct DecimalCarrier {
    CHAOS_IL2CPP_UINT32 flags;
    CHAOS_IL2CPP_UINT64 lo64;
    CHAOS_IL2CPP_UINT32 hi32;
};

inline void chaos_decimal_ctor_int32(DecimalCarrier* carrier, CHAOS_IL2CPP_INT32 value) {
    if (value < 0) {
        carrier->flags = 0x80000000u;
        carrier->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(-static_cast<CHAOS_IL2CPP_INT64>(value));
    } else {
        carrier->flags = 0u;
        carrier->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(value);
    }
    carrier->hi32 = 0u;
}

// Overload: intptr_t convenience wrapper for older generated code that
// passes an intptr_t (treated as DecimalCarrier*) rather than a typed pointer.
inline void chaos_decimal_ctor_int32(CHAOS_IL2CPP_INTPTR carrier_ptr, CHAOS_IL2CPP_INT32 value) {
    chaos_decimal_ctor_int32(reinterpret_cast<DecimalCarrier*>(carrier_ptr), value);
}

}  // namespace chaos::il2cpp::runtime_core

// File-scope using declaration so older generated code (inside anonymous
// namespaces) can call chaos_decimal_ctor_int32 as a bare identifier.
using chaos::il2cpp::runtime_core::chaos_decimal_ctor_int32;

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
