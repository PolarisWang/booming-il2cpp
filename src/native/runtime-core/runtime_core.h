#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include "runtime_abi.h"
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

struct ManagedExceptionCarrier {
    ExceptionHandle exception;
};

using EngineLifecycleCallback = void (*)(const char* phase_utf8, void* user_data);
using FinalizerCallback = void (*)(void* object_instance);

enum class RuntimeMode {
    Aot = 0,
    Mixed = 1,
};

enum class ValueTypeKernelBackendKind {
    Generic = 0,
    Platform = 1,
    Intrinsic = 2,
};

const RuntimeAbiV0* GetRuntimeAbiV0();
RuntimeMode GetRuntimeMode();
void SetRuntimeMode(RuntimeMode mode);
bool IsMixedMode();
void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    size_t value_size);
RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    size_t out_value_size);
bool ArrayStoreReference(
    void* array_instance,
    uintptr_t index,
    void* value);
void* ArrayLoadReference(
    void* array_instance,
    uintptr_t index);
bool ArrayCopyReferenceRange(
    void* source_array_instance,
    uintptr_t source_index,
    void* target_array_instance,
    uintptr_t target_index,
    uintptr_t length);
bool ArrayClearReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length);
bool ArrayReverseReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length);
int32_t EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8);
uintptr_t CreateEngineObjectHandle(void* object_instance);
void* ResolveEngineObjectHandle(uintptr_t handle);
bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data);
bool DispatchEngineLifecycleCallbacks(const char* phase_utf8);
bool IsMainThreadLane();
bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    int32_t delta,
    int32_t* out_value);
bool MonitorEnter(void* monitor_target);
bool MonitorExit(void* monitor_target);
bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state);
size_t ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    size_t root_size);
bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer);
size_t DrainFinalizerQueue(RuntimeState* runtime_state);
ValueTypeKernelBackendKind GetCharClassificationKernelBackend();
ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend();
ValueTypeKernelBackendKind GetBitArithmeticKernelBackend();
ValueTypeKernelBackendKind GetTickArithmeticKernelBackend();
bool CharIsAscii(std::uint16_t value);
bool CharIsAsciiDigit(std::uint16_t value);
bool CharIsAsciiHexDigit(std::uint16_t value);
bool CharIsAsciiHexDigitLower(std::uint16_t value);
bool CharIsAsciiHexDigitUpper(std::uint16_t value);
bool CharIsAsciiLetter(std::uint16_t value);
bool CharIsAsciiLetterLower(std::uint16_t value);
bool CharIsAsciiLetterOrDigit(std::uint16_t value);
bool CharIsAsciiLetterUpper(std::uint16_t value);
bool CharIsBetween(std::uint16_t value, std::uint16_t lower_bound, std::uint16_t upper_bound);
bool CharIsHighSurrogate(std::uint16_t value);
bool CharIsLatin1(std::uint16_t value);
bool CharIsLowSurrogate(std::uint16_t value);
bool CharIsSeparatorLatin1(std::uint16_t value);
bool CharIsSurrogate(std::uint16_t value);
bool CharIsSurrogatePair(std::uint16_t high_surrogate, std::uint16_t low_surrogate);
bool CharIsWhiteSpaceLatin1(std::uint16_t value);
int32_t CharCompare(std::uint16_t left_value, std::uint16_t right_value);
bool CharEquals(std::uint16_t left_value, std::uint16_t right_value);
bool HalfIsFinite(std::uint16_t value);
bool HalfIsEvenInteger(std::uint16_t value);
bool HalfIsNaN(std::uint16_t value);
bool HalfIsNaNOrZero(std::uint16_t value);
bool HalfIsNegative(std::uint16_t value);
bool HalfIsNegativeInfinity(std::uint16_t value);
bool HalfIsNormal(std::uint16_t value);
bool HalfIsOddInteger(std::uint16_t value);
bool HalfIsPositive(std::uint16_t value);
bool HalfIsPositiveInfinity(std::uint16_t value);
bool HalfIsPow2(std::uint16_t value);
bool HalfIsRealNumber(std::uint16_t value);
bool HalfIsSubnormal(std::uint16_t value);
bool HalfIsZero(std::uint16_t value);
bool HalfIsInteger(std::uint16_t value);
bool HalfIsInfinity(std::uint16_t value);
int32_t HalfCompare(std::uint16_t left_value, std::uint16_t right_value);
bool HalfEquals(std::uint16_t left_value, std::uint16_t right_value);
bool HalfOperatorEquals(std::uint16_t left_value, std::uint16_t right_value);
bool HalfOperatorLessThan(std::uint16_t left_value, std::uint16_t right_value);
bool HalfOperatorLessThanOrEqual(std::uint16_t left_value, std::uint16_t right_value);
bool HalfOperatorGreaterThan(std::uint16_t left_value, std::uint16_t right_value);
bool HalfOperatorGreaterThanOrEqual(std::uint16_t left_value, std::uint16_t right_value);
int32_t SingleCompare(float left_value, float right_value);
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
int32_t DoubleCompare(double left_value, double right_value);
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
int32_t NFloatCompare(double left_value, double right_value);
bool NFloatEquals(double left_value, double right_value);
int32_t Int128Compare(const void* left_value, const void* right_value);
bool Int128Equals(const void* left_value, const void* right_value);
int32_t UInt128Compare(const void* left_value, const void* right_value);
bool UInt128Equals(const void* left_value, const void* right_value);
int32_t IntPtrCompare(std::intptr_t left_value, std::intptr_t right_value);
bool IntPtrEquals(std::intptr_t left_value, std::intptr_t right_value);
int32_t UIntPtrCompare(std::uintptr_t left_value, std::uintptr_t right_value);
bool UIntPtrEquals(std::uintptr_t left_value, std::uintptr_t right_value);
int32_t DateTimeCompareTicks(const void* left_value, const void* right_value);
bool DateTimeEqualsTicks(const void* left_value, const void* right_value);
int32_t TimeSpanCompareTicks(const void* left_value, const void* right_value);
bool TimeSpanEqualsTicks(const void* left_value, const void* right_value);
int32_t DateOnlyCompareDayNumber(std::int32_t left_value, std::int32_t right_value);
bool DateOnlyEqualsDayNumber(std::int32_t left_value, std::int32_t right_value);
int32_t TimeOnlyCompareTicksValue(std::int64_t left_value, std::int64_t right_value);
bool TimeOnlyEqualsTicksValue(std::int64_t left_value, std::int64_t right_value);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
