#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

static float VectorMinElement(float left_value, float right_value) { return CHAOS_IL2CPP_FMIN(left_value, right_value); }
static float VectorMaxElement(float left_value, float right_value) { return CHAOS_IL2CPP_FMAX(left_value, right_value); }

static bool HalfIsFiniteGeneric(CHAOS_IL2CPP_UINT16 value) { return (value & 0x7C00u) != 0x7C00u; }
static bool HalfIsNaNGeneric(CHAOS_IL2CPP_UINT16 value) { return (value & 0x7C00u) == 0x7C00u && (value & 0x03FFu) != 0u; }
static bool HalfIsInfinityGeneric(CHAOS_IL2CPP_UINT16 value) { return (value & 0x7FFFu) == 0x7C00u; }
static float HalfToFloatValue(CHAOS_IL2CPP_UINT16 value);

bool HalfIsFinite(CHAOS_IL2CPP_UINT16 value) { return HalfIsFiniteGeneric(value); }

bool HalfIsEvenInteger(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsInteger(value) && CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(HalfToFloatValue(value), 2.0f)) == 0.0f;
}

bool HalfIsNaN(CHAOS_IL2CPP_UINT16 value) { return HalfIsNaNGeneric(value); }
bool HalfIsNaNOrZero(CHAOS_IL2CPP_UINT16 value) { return (value & 0x7FFFu) == 0u || HalfIsNaN(value); }
bool HalfIsNegative(CHAOS_IL2CPP_UINT16 value) { return (value & 0x8000u) == 0x8000u; }
bool HalfIsNegativeInfinity(CHAOS_IL2CPP_UINT16 value) { return value == 0xFC00u; }

bool HalfIsNormal(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 abs_value = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x7FFFu);
    return abs_value < 0x7C00u && abs_value != 0u && (abs_value & 0x7C00u) != 0u;
}

bool HalfIsOddInteger(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsInteger(value) && CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(HalfToFloatValue(value), 2.0f)) == 1.0f;
}

bool HalfIsPositive(CHAOS_IL2CPP_UINT16 value) { return !HalfIsNegative(value); }
bool HalfIsPositiveInfinity(CHAOS_IL2CPP_UINT16 value) { return value == 0x7C00u; }

bool HalfIsPow2(CHAOS_IL2CPP_UINT16 value) {
    if (!HalfIsFinite(value)) return false;
    const float float_value = HalfToFloatValue(value);
    if (!(float_value > 0.0f)) return false;
    int exponent = 0;
    return CHAOS_IL2CPP_FREXP(float_value, &exponent) == 0.5f;
}

bool HalfIsRealNumber(CHAOS_IL2CPP_UINT16 value) { return !HalfIsNaN(value); }

bool HalfIsSubnormal(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 abs_value = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x7FFFu);
    return abs_value != 0u && (abs_value & 0x7C00u) == 0u;
}

bool HalfIsZero(CHAOS_IL2CPP_UINT16 value) { return (value & 0x7FFFu) == 0u; }

bool HalfIsInteger(CHAOS_IL2CPP_UINT16 value) {
    if (!HalfIsFinite(value)) return false;
    return CHAOS_IL2CPP_TRUNC(HalfToFloatValue(value)) == HalfToFloatValue(value);
}

bool HalfIsInfinity(CHAOS_IL2CPP_UINT16 value) { return HalfIsInfinityGeneric(value); }

static float HalfToFloatValue(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 exponent = static_cast<CHAOS_IL2CPP_UINT16>((value >> 10) & 0x1Fu);
    const CHAOS_IL2CPP_UINT16 mantissa = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x03FFu);
    const bool negative = (value & 0x8000u) != 0u;

    float result = 0.0f;
    if (exponent == 0u) {
        result = mantissa == 0u ? 0.0f : CHAOS_IL2CPP_LDEXP(static_cast<float>(mantissa), -24);
    } else if (exponent == 0x1Fu) {
        result = mantissa == 0u
            ? CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float)
            : CHAOS_IL2CPP_NUMERIC_LIMITS_QUIET_NAN(float);
    } else {
        result = CHAOS_IL2CPP_LDEXP(1.0f + (static_cast<float>(mantissa) / 1024.0f), static_cast<int>(exponent) - 15);
    }

    return negative ? -result : result;
}

CHAOS_IL2CPP_INT32 HalfCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    const bool left_is_nan = HalfIsNaN(left_value);
    const bool right_is_nan = HalfIsNaN(right_value);
    if (left_is_nan && right_is_nan) return 0;
    if (left_is_nan) return -1;
    if (right_is_nan) return 1;

    const float left = HalfToFloatValue(left_value);
    const float right = HalfToFloatValue(right_value);
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

bool HalfEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return left_value == right_value ||
           (HalfIsZero(left_value) && HalfIsZero(right_value)) ||
           (HalfIsNaN(left_value) && HalfIsNaN(right_value));
}

static bool HalfAreZero(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return ((left_value | right_value) & 0x7FFFu) == 0u;
}

bool HalfOperatorEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return !HalfIsNaN(left_value) && !HalfIsNaN(right_value) &&
           (left_value == right_value || HalfAreZero(left_value, right_value));
}

bool HalfOperatorLessThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) return false;
    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value))
        return left_is_negative && !HalfAreZero(left_value, right_value);
    return left_value != right_value && ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorLessThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) return false;
    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value))
        return left_is_negative || HalfAreZero(left_value, right_value);
    return left_value == right_value || ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorGreaterThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return HalfOperatorLessThan(right_value, left_value);
}

bool HalfOperatorGreaterThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return HalfOperatorLessThanOrEqual(right_value, left_value);
}

template <typename T>
static CHAOS_IL2CPP_INT32 FloatingCompareGeneric(T left_value, T right_value) {
    const bool left_is_nan = CHAOS_IL2CPP_ISNAN(left_value);
    const bool right_is_nan = CHAOS_IL2CPP_ISNAN(right_value);
    if (left_is_nan && right_is_nan) return 0;
    if (left_is_nan) return -1;
    if (right_is_nan) return 1;
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

template <typename T>
static bool FloatingEqualsGeneric(T left_value, T right_value) {
    return left_value == right_value || (CHAOS_IL2CPP_ISNAN(left_value) && CHAOS_IL2CPP_ISNAN(right_value));
}

template <typename T>
static bool FloatingIsIntegerGeneric(T value) {
    return CHAOS_IL2CPP_ISFINITE(value) && CHAOS_IL2CPP_TRUNC(value) == value;
}

template <typename T>
static bool FloatingIsEvenIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) && CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(value, static_cast<T>(2))) == static_cast<T>(0);
}

template <typename T>
static bool FloatingIsOddIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) && CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(value, static_cast<T>(2))) == static_cast<T>(1);
}

template <typename T>
static bool FloatingIsPow2Generic(T value) {
    if (!CHAOS_IL2CPP_ISFINITE(value) || !(value > static_cast<T>(0))) return false;
    int exponent = 0;
    return CHAOS_IL2CPP_FREXP(value, &exponent) == static_cast<T>(0.5);
}

// Single classification
static bool SingleIsFiniteGeneric(float value) { return CHAOS_IL2CPP_ISFINITE(value); }
static bool SingleIsFiniteIntrinsicImpl(float value) { return (BitCastSingleToUInt32(value) & 0x7F800000u) != 0x7F800000u; }

bool SingleIsFinite(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsFiniteIntrinsicImpl(value) : SingleIsFiniteGeneric(value);
}

static bool SingleIsNaNGeneric(float value) { return CHAOS_IL2CPP_ISNAN(value); }
static bool SingleIsNaNIntrinsicImpl(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0u;
}

bool SingleIsNaN(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsNaNIntrinsicImpl(value) : SingleIsNaNGeneric(value);
}

bool SingleIsInfinity(float value) { return CHAOS_IL2CPP_ISINF(value); }
bool SingleIsInfinityIntrinsicImpl(float value) { return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0x7F800000u; }

CHAOS_IL2CPP_INT32 SingleCompare(float left_value, float right_value) { return FloatingCompareGeneric(left_value, right_value); }
bool SingleEquals(float left_value, float right_value) { return FloatingEqualsGeneric(left_value, right_value); }
bool SingleIsEvenInteger(float value) { return FloatingIsEvenIntegerGeneric(value); }
bool SingleIsNaNOrZero(float value) { return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0u || SingleIsNaN(value); }
bool SingleIsNegative(float value) { return (BitCastSingleToUInt32(value) & 0x80000000u) != 0u; }
bool SingleIsNegativeInfinity(float value) { return BitCastSingleToUInt32(value) == 0xFF800000u; }

bool SingleIsNormal(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) < 0x7F800000u && (bits & 0x7FFFFFFFu) != 0u && (bits & 0x7F800000u) != 0u;
}

bool SingleIsOddInteger(float value) { return FloatingIsOddIntegerGeneric(value); }
bool SingleIsPositive(float value) { return (BitCastSingleToUInt32(value) & 0x80000000u) == 0u; }
bool SingleIsPositiveInfinity(float value) { return BitCastSingleToUInt32(value) == 0x7F800000u; }
bool SingleIsPow2(float value) { return FloatingIsPow2Generic(value); }
bool SingleIsRealNumber(float value) { return !SingleIsNaN(value); }

bool SingleIsSubnormal(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) != 0u && (bits & 0x7F800000u) == 0u;
}

bool SingleIsZero(float value) { return value == 0.0f; }
bool SingleIsInteger(float value) { return FloatingIsIntegerGeneric(value); }

// Double classification
bool DoubleIsFinite(float value); // forward decl to match header — actual impl below
static bool DoubleIsFiniteGeneric(double value) { return CHAOS_IL2CPP_ISFINITE(value); }
static bool DoubleIsFiniteIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FF0000000000000ull) != 0x7FF0000000000000ull;
}

bool DoubleIsFinite(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsFiniteIntrinsicImpl(value) : DoubleIsFiniteGeneric(value);
}

static bool DoubleIsNaNGeneric(double value) { return CHAOS_IL2CPP_ISNAN(value); }
static bool DoubleIsNaNIntrinsicImpl(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FF0000000000000ull) == 0x7FF0000000000000ull && (bits & 0x000FFFFFFFFFFFFFull) != 0u;
}

bool DoubleIsNaN(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsNaNIntrinsicImpl(value) : DoubleIsNaNGeneric(value);
}

bool DoubleIsInfinity(double value) { return CHAOS_IL2CPP_ISINF(value); }
static bool DoubleIsInfinityIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0x7FF0000000000000ull;
}

CHAOS_IL2CPP_INT32 DoubleCompare(double left_value, double right_value) { return FloatingCompareGeneric(left_value, right_value); }
bool DoubleEquals(double left_value, double right_value) { return FloatingEqualsGeneric(left_value, right_value); }
bool DoubleIsEvenInteger(double value) { return FloatingIsEvenIntegerGeneric(value); }
bool DoubleIsNaNOrZero(double value) { return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0u || DoubleIsNaN(value); }
bool DoubleIsNegative(double value) { return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) != 0u; }
bool DoubleIsNegativeInfinity(double value) { return BitCastDoubleToUInt64(value) == 0xFFF0000000000000ull; }

bool DoubleIsNormal(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) < 0x7FF0000000000000ull &&
           (bits & 0x7FFFFFFFFFFFFFFFull) != 0u && (bits & 0x7FF0000000000000ull) != 0u;
}

bool DoubleIsOddInteger(double value) { return FloatingIsOddIntegerGeneric(value); }
bool DoubleIsPositive(double value) { return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) == 0u; }
bool DoubleIsPositiveInfinity(double value) { return BitCastDoubleToUInt64(value) == 0x7FF0000000000000ull; }
bool DoubleIsPow2(double value) { return FloatingIsPow2Generic(value); }
bool DoubleIsRealNumber(double value) { return !DoubleIsNaN(value); }

bool DoubleIsSubnormal(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) != 0u && (bits & 0x7FF0000000000000ull) == 0u;
}

bool DoubleIsZero(double value) { return value == 0.0; }
bool DoubleIsInteger(double value) { return FloatingIsIntegerGeneric(value); }

// NFloat delegates
#define DEFINE_NFLOAT_BOOL_DELEGATE(name) \
    bool NFloat##name(double value) { return Double##name(value); }
#define DEFINE_NFLOAT_CMP_DELEGATE(name) \
    CHAOS_IL2CPP_INT32 NFloat##name(double left, double right) { return Double##name(left, right); }

DEFINE_NFLOAT_BOOL_DELEGATE(IsFinite);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNaN);
DEFINE_NFLOAT_BOOL_DELEGATE(IsInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsEvenInteger);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNegative);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNegativeInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNormal);
DEFINE_NFLOAT_BOOL_DELEGATE(IsOddInteger);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPositive);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPositiveInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPow2);
DEFINE_NFLOAT_BOOL_DELEGATE(IsRealNumber);
DEFINE_NFLOAT_BOOL_DELEGATE(IsSubnormal);
DEFINE_NFLOAT_BOOL_DELEGATE(IsInteger);

DEFINE_NFLOAT_CMP_DELEGATE(Compare);

bool NFloatEquals(double left, double right) { return DoubleEquals(left, right); }

#undef DEFINE_NFLOAT_BOOL_DELEGATE
#undef DEFINE_NFLOAT_CMP_DELEGATE

// Int128 classification
static CHAOS_IL2CPP_INT32 Int128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) return 0;
    Int128Layout left = {}; Int128Layout right = {};
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left.upper < right.upper) return -1;
    if (left.upper > right.upper) return 1;
    if (left.lower < right.lower) return -1;
    if (left.lower > right.lower) return 1;
    return 0;
}

static CHAOS_IL2CPP_INT32 Int128CompareIntrinsicImpl(const void* left_value, const void* right_value) {
    return Int128CompareGeneric(left_value, right_value);
}

CHAOS_IL2CPP_INT32 Int128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? Int128CompareIntrinsicImpl(left_value, right_value) : Int128CompareGeneric(left_value, right_value);
}

bool Int128Equals(const void* left_value, const void* right_value) { return Int128Compare(left_value, right_value) == 0; }

static CHAOS_IL2CPP_INT32 UInt128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) return 0;
    UInt128Layout left = {}; UInt128Layout right = {};
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left.upper < right.upper) return -1;
    if (left.upper > right.upper) return 1;
    if (left.lower < right.lower) return -1;
    if (left.lower > right.lower) return 1;
    return 0;
}

CHAOS_IL2CPP_INT32 UInt128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? UInt128CompareGeneric(left_value, right_value) : UInt128CompareGeneric(left_value, right_value);
}

bool UInt128Equals(const void* left_value, const void* right_value) { return UInt128Compare(left_value, right_value) == 0; }

CHAOS_IL2CPP_INT32 IntPtrCompare(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value) {
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

bool IntPtrEquals(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value) { return left_value == right_value; }

CHAOS_IL2CPP_INT32 UIntPtrCompare(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value) {
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

bool UIntPtrEquals(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value) { return left_value == right_value; }

CHAOS_IL2CPP_INT32 DateTimeCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) return 0;
    CHAOS_IL2CPP_UINT64 left = 0u, right = 0u;
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    left &= kDateTimeTicksMask; right &= kDateTimeTicksMask;
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

bool DateTimeEqualsTicks(const void* left_value, const void* right_value) { return DateTimeCompareTicks(left_value, right_value) == 0; }

CHAOS_IL2CPP_INT32 TimeSpanCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) return 0;
    CHAOS_IL2CPP_INT64 left = 0, right = 0;
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

bool TimeSpanEqualsTicks(const void* left_value, const void* right_value) { return TimeSpanCompareTicks(left_value, right_value) == 0; }

CHAOS_IL2CPP_INT32 DateOnlyCompareDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value) {
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

bool DateOnlyEqualsDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value) { return left_value == right_value; }

CHAOS_IL2CPP_INT32 TimeOnlyCompareTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value) {
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    return 0;
}

bool TimeOnlyEqualsTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value) { return left_value == right_value; }

}  // namespace chaos::il2cpp::runtime_core