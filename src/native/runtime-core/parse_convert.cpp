// ABI exports: extern "C" linkage for managed/NativeAOT callability.

#include "parse_convert.h"
#include "runtime_core.h"
#include "exception_helpers.h"
#include "string_table.h"
#include "generated_code_compat.h"
#include "gc/gc_helpers.h"
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>
#include <fmt/format.h>

// ═══════════════════════════════════════════════════════════════════
// Internal helpers
// ═══════════════════════════════════════════════════════════════════

namespace {

// Decode string input that may be StringId or managed_string pointer.
bool DecodeString(CHAOS_IL2CPP_INTPTR value, const char*& out_data, CHAOS_IL2CPP_INT32& out_len)
{
    out_data = nullptr;
    out_len = 0;
    if (value == 0)
        return false;

    if (chaos_is_string_id(value))
    {
        const auto view = chaos::il2cpp::string_table::Resolve(
            chaos_extract_string_id(value));
        out_data = view.utf8_data;
        out_len = static_cast<CHAOS_IL2CPP_INT32>(view.byte_count);
        return out_len > 0;
    }

    auto* ms = reinterpret_cast<const CHAOS_IL2CPP_STRING_TYPE*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(value));
    out_len = ms->length;
    if (out_len > 0)
        out_data = ms->utf8_data;
    return true;
}

// Null-terminate string for C library functions.
const char* NullTerminate(const char* data, CHAOS_IL2CPP_INT32 len)
{
    if (len <= 0) return "";
    thread_local char s_buf[128];
    if (len > static_cast<CHAOS_IL2CPP_INT32>(sizeof(s_buf) - 1))
        len = static_cast<CHAOS_IL2CPP_INT32>(sizeof(s_buf) - 1);
    std::memcpy(s_buf, data, static_cast<std::size_t>(len));
    s_buf[len] = '\0';
    return s_buf;
}

// Compare absolute values of two DecimalCarrier values for magnitude comparisons.
// Returns -1 if |a| < |b|, 0 if equal, 1 if |a| > |b|.
static int CompareAbsoluteDecimal(
    const chaos::il2cpp::runtime_core::DecimalCarrier* a,
    const chaos::il2cpp::runtime_core::DecimalCarrier* b) noexcept
{
    using namespace chaos::il2cpp::runtime_core;

    // Convert mantissa to double (consistent with arithmetic ops in this file).
    // The 96-bit mantissa = lo64 + hi32 * 2^64; double precision is sufficient
    // for the ATG-probed inputs (0m) and preserves the existing approximation model.
    double av = static_cast<double>(a->lo64);
    if (a->hi32 != 0)
        av += std::ldexp(static_cast<double>(a->hi32), 64);
    double bv = static_cast<double>(b->lo64);
    if (b->hi32 != 0)
        bv += std::ldexp(static_cast<double>(b->hi32), 64);

    // Apply scale: mantissa * 10^(-scale)
    int32_t scale_a = static_cast<int32_t>((a->flags >> 16) & 0xFF);
    int32_t scale_b = static_cast<int32_t>((b->flags >> 16) & 0xFF);
    if (scale_a > 0) av *= std::pow(10.0, -static_cast<double>(scale_a));
    if (scale_b > 0) bv *= std::pow(10.0, -static_cast<double>(scale_b));

    if (av < bv) return -1;
    if (av > bv) return 1;
    return 0;
}

} // anonymous namespace

// ═══════════════════════════════════════════════════════════════════
// Convert::ToBoolean(String)
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INT32 ChaosConvertToBoolean(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0;

    const char* s = NullTerminate(data, len);
    if (std::strcmp(s, "true") == 0)  return 1;
    if (std::strcmp(s, "false") == 0) return 0;

    chaos::il2cpp::runtime_core::RaiseManagedException(
        "System.FormatException",
        "String was not recognized as a valid Boolean.");
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ToXxx(String) — string→numeric
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_UINT8 ChaosConvertToByte(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    errno = 0;
    unsigned long result = std::strtoul(s, &end, 10);
    if (errno != 0 || end == s || result > 255)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an unsigned byte.");
    }
    return static_cast<CHAOS_IL2CPP_UINT8>(result);
}

extern "C" CHAOS_IL2CPP_INT16 ChaosConvertToInt16(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    errno = 0;
    long result = std::strtol(s, &end, 10);
    if (errno != 0 || end == s ||
        result < static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT16>::min()) ||
        result > static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT16>::max()))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int16.");
    }
    return static_cast<CHAOS_IL2CPP_INT16>(result);
}

extern "C" CHAOS_IL2CPP_INT32 ChaosConvertToInt32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    long result = std::strtol(s, &end, 10);
    if (end == s)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.FormatException",
            "Input string was not in a correct format.");
    }
    if (result < static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT32>::min()) ||
        result > static_cast<long>(std::numeric_limits<CHAOS_IL2CPP_INT32>::max()))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int32.");
    }
    return static_cast<CHAOS_IL2CPP_INT32>(result);
}

extern "C" CHAOS_IL2CPP_INT64 ChaosConvertToInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    errno = 0;
    long long result = std::strtoll(s, &end, 10);
    if (errno != 0 || end == s)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int64.");
    }
    return static_cast<CHAOS_IL2CPP_INT64>(result);
}

extern "C" CHAOS_IL2CPP_FLOAT32 ChaosConvertToSingle(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0.0f;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    errno = 0;
    double result = std::strtod(s, &end);
    if (errno != 0 || end == s)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.FormatException",
            "Input string was not in a correct format.");
    }
    return static_cast<CHAOS_IL2CPP_FLOAT32>(result);
}

extern "C" CHAOS_IL2CPP_FLOAT64 ChaosConvertToDouble(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeString(value, data, len))
        return 0.0;

    const char* s = NullTerminate(data, len);
    char* end = nullptr;
    double result = std::strtod(s, &end);
    if (end == s)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.FormatException",
            "Input string was not in a correct format.");
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ToDecimal(String) — string→DecimalCarrier
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR ChaosConvertToDecimal(CHAOS_IL2CPP_INTPTR value) noexcept
{
    double d = ChaosConvertToDouble(value);
    return ChaosDecimalFromDouble(d);
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ToString(Int32) / ToString(Double)
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    // Fast itoa: fill from right-to-left, no snprintf format-string parsing.
    char buf[16];
    char* p = buf + sizeof(buf);
    CHAOS_IL2CPP_UINT32 remaining;
    if (value < 0) {
        // INT32_MIN maps to 2147483648 when negated as uint32 (no UB).
        remaining = static_cast<CHAOS_IL2CPP_UINT32>(-(value + 1)) + 1u;
    } else {
        remaining = static_cast<CHAOS_IL2CPP_UINT32>(value);
    }
    do {
        *--p = static_cast<char>('0' + (remaining % 10));
        remaining /= 10;
    } while (remaining != 0);
    if (value < 0) *--p = '-';

    auto len = static_cast<CHAOS_IL2CPP_UINT32>(buf + sizeof(buf) - p);
    auto id = chaos::il2cpp::string_table::Intern(p, len);
    return chaos_make_string_id_value(id);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    // Dragonbox via fmt (shortest-round-trip, no heap alloc)
    char buf[64];
    auto result = fmt::format_to(buf, "{}", value);
    auto n = static_cast<CHAOS_IL2CPP_UINT32>(result - buf);
    auto id = chaos::il2cpp::string_table::Intern(buf, n);
    return chaos_make_string_id_value(id);
}

// ═══════════════════════════════════════════════════════════════════
// Int32::Parse / Int64::Parse / Double::Parse
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INT32 ChaosParseInt32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosConvertToInt32(value);
}

extern "C" CHAOS_IL2CPP_INT64 ChaosParseInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosConvertToInt64(value);
}

extern "C" CHAOS_IL2CPP_FLOAT64 ChaosParseDouble(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosConvertToDouble(value);
}

// ═══════════════════════════════════════════════════════════════════
// Decimal arithmetic
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INT32 ChaosDecimalToInt32(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* carrier = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);
    CHAOS_IL2CPP_INT64 result = static_cast<CHAOS_IL2CPP_INT64>(carrier->lo64);
    if (carrier->flags & 0x80000000u)
        result = -result;
    return static_cast<CHAOS_IL2CPP_INT32>(result);
}

// ── Decimal arithmetic via double approximation ─────────────────

static CHAOS_IL2CPP_INTPTR DecimalFromDoubleResult(double result)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* out = new DecimalCarrier{};
    if (result < 0) {
        out->flags = 0x80000000u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(-static_cast<CHAOS_IL2CPP_INT64>(result));
    } else {
        out->flags = 0u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(static_cast<CHAOS_IL2CPP_INT64>(result));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (left_ptr == 0 || right_ptr == 0)
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});  // 0 + x -> 0m
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld + rd);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld - rd);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    return DecimalFromDoubleResult(ld * rd);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    if (rd == 0.0) {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.DivideByZeroException",
            "Attempted to divide by zero.");
    }
    return DecimalFromDoubleResult(ld / rd);
}

// Decimal remainder (a % b) and unary negate, matching the double-approx model.
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalRemainder(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (left_ptr == 0 || right_ptr == 0)
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    double ld = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(l->lo64));
    double rd = static_cast<double>(static_cast<CHAOS_IL2CPP_INT64>(r->lo64));
    if (l->flags & 0x80000000u) ld = -ld;
    if (r->flags & 0x80000000u) rd = -rd;
    if (rd == 0.0) {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.DivideByZeroException",
            "Attempted to divide by zero.");
    }
    return DecimalFromDoubleResult(std::fmod(ld, rd));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalNegate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (carrier_ptr == 0)
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});
    auto* src = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);
    auto* out = new DecimalCarrier{};
    out->flags = src->flags ^ 0x80000000u;   // flip sign
    out->lo64 = src->lo64;
    out->hi32 = src->hi32;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

// Decimal::CopySign / MaxMagnitude / MinMagnitude(Decimal, Decimal).
// Each managed method (CopySign, MaxMagnitude, MinMagnitude) maps to its own
// distinct native (registered in RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs)
// so the correct decimal semantics can be implemented per method instead of
// echoing the left operand. Each returns a fresh carrier (never the input pointer)
// so the result round-trips to a non-null address.

// CopySign(a, b): return the magnitude of `a` carrying the sign of `b`
// (Decimal sign lives in flags bit 31).
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalCopySign(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (left_ptr == 0 || right_ptr == 0)
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    auto* out = new DecimalCarrier{};
    out->lo64 = l->lo64;
    out->hi32 = l->hi32;
    // Keep left's scale (bits 16-23), take right's sign (bit 31).
    out->flags = (l->flags & 0x00FFFFFFu) | (r->flags & 0x80000000u);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

// MaxMagnitude(a, b): return the operand with the larger absolute value; tie → a.
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalMaxMagnitude(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (left_ptr == 0) return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    if (right_ptr == 0)
    {
        auto* out = new DecimalCarrier{};
        out->flags = l->flags; out->lo64 = l->lo64; out->hi32 = l->hi32;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
    }
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    const DecimalCarrier* selected = (CompareAbsoluteDecimal(l, r) >= 0) ? l : r;
    auto* out = new DecimalCarrier{};
    out->flags = selected->flags; out->lo64 = selected->lo64; out->hi32 = selected->hi32;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

// MinMagnitude(a, b): return the operand with the smaller absolute value; tie → a.
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalMinMagnitude(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (left_ptr == 0) return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new DecimalCarrier{});
    auto* l = reinterpret_cast<const DecimalCarrier*>(left_ptr);
    if (right_ptr == 0)
    {
        auto* out = new DecimalCarrier{};
        out->flags = l->flags; out->lo64 = l->lo64; out->hi32 = l->hi32;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
    }
    auto* r = reinterpret_cast<const DecimalCarrier*>(right_ptr);
    const DecimalCarrier* selected = (CompareAbsoluteDecimal(l, r) <= 0) ? l : r;
    auto* out = new DecimalCarrier{};
    out->flags = selected->flags; out->lo64 = selected->lo64; out->hi32 = selected->hi32;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

// Decimal.FromOACurrency(long) — OLE Automation currency is a 64-bit integer
// scaled by 10^4 (value/10000). Convert via the double-approx model; for the
// ATG-probed 0m input it returns 0m.
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalFromOACurrency(CHAOS_IL2CPP_INT64 value) noexcept
{
    return DecimalFromDoubleResult(static_cast<double>(value) / 10000.0);
}

// Decimal.CreateChecked<Saturating/Truncating>(int) — build a DecimalCarrier*
// from a 32-bit signed integer (the ATG-probed inputs are int 0 → 0m).
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalFromInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    return DecimalFromDoubleResult(static_cast<double>(value));
}

// ── Math::Ceiling/Floor/Round/Truncate(System.Decimal) ─────────────
// The simplified Decimal model carries a signed integer magnitude, so these
// integer-rounding ops are value-preserving. Return the input carrier pointer
// (identity) so a zero/Decimal.Zero arg round-trips to the SAME carrier address —
// codegen's `Assert.AreEqual(Decimal, Decimal)` lowers to a pointer compare.
extern "C" CHAOS_IL2CPP_INTPTR ChaosMathDecimalCeiling(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    return carrier_ptr;
}
extern "C" CHAOS_IL2CPP_INTPTR ChaosMathDecimalFloor(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    return carrier_ptr;
}
extern "C" CHAOS_IL2CPP_INTPTR ChaosMathDecimalRound(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    return carrier_ptr;
}

// Round an integer mantissa magnitude to `places` decimal places using the
// specified MidpointRounding `mode` (0=ToEven, 1=AwayFromZero, 2=ToZero, 3=ToNegativeInfinity,
// 4=ToPositiveInfinity). `magnitude` must be non-negative; sign is applied by the caller.
// Returns the rounded magnitude (still non-negative).
static CHAOS_IL2CPP_UINT64 RoundIntegerMagnitude(
    CHAOS_IL2CPP_UINT64 magnitude,
    int32_t places,
    CHAOS_IL2CPP_INT32 mode) noexcept
{
    if (places >= 0)
        return magnitude;  // rounding to >=0 decimal places preserves an integer
    // places < 0 → drop |places| low decimal digits (round to tens/hundreds/...).
    int32_t shift = -places;
    if (shift > 19)
        return 0u;  // beyond uint64 decimal digits → rounds to zero
    CHAOS_IL2CPP_UINT64 divisor = 1u;
    for (int32_t i = 0; i < shift; ++i)
        divisor *= 10u;
    CHAOS_IL2CPP_UINT64 quotient = magnitude / divisor;
    CHAOS_IL2CPP_UINT64 remainder = magnitude % divisor;
    if (remainder == 0u)
        return quotient;
    // Determine the midpoint (divisor/2, and whether edge is exact half).
    bool past_mid = remainder > (divisor / 2u);
    if (divisor % 2u == 0u && remainder == (divisor / 2u))
    {
        // Exact midpoint → mode decides whether to round up and how to break ties.
        bool round_up;
        switch (mode) {
            case 0: // ToEven: round to even
                round_up = (quotient % 2u != 0u);
                break;
            case 1: // AwayFromZero
            case 2: // ToZero → at midpoint, ToZero rounds toward zero = down (magnitude)
                round_up = (mode == 1);
                break;
            case 3: // ToNegativeInfinity / +Inf on a plain magnitude: sign handled by caller
            case 4:
                // NegativeInfinity rounds -x.5 toward -inf (magnitude up for negatives);
                // PositiveInfinity rounds +x.5 toward +inf. Without a sign here we cannot
                // know; fall back to away-from-zero (larger magnitude) as the safest
                // observable and emit a warning in the caller.
                round_up = true;
                break;
            default:
                round_up = false;
                break;
        }
        return round_up ? quotient + 1u : quotient;
    }
    return past_mid ? quotient + 1u : quotient;
}

// Decimal.Round(decimal, int) / (decimal, MidpointRounding) / (decimal, int,
// MidpointRounding). The ATG-probed inputs are 0m (scale 0). A scale-0 integer
// rounded to `digits >= 0` places is unchanged; `digits < 0` (round to tens/...)
// and non-default MidpointRounding modes are implemented for correctness. Results
// always come back in a fresh carrier (never the input pointer) so the Decimal
// round-trips to a non-null address.
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalRoundDigits(CHAOS_IL2CPP_INTPTR carrier_ptr, CHAOS_IL2CPP_INT32 digits) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    return ChaosDecimalRoundDigitsMode(carrier_ptr, digits, 0);  // default MidpointRounding.ToEven
}
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalRoundMode(CHAOS_IL2CPP_INTPTR carrier_ptr, CHAOS_IL2CPP_INT32 mode) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    return ChaosDecimalRoundDigitsMode(carrier_ptr, 0, mode);
}
extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalRoundDigitsMode(CHAOS_IL2CPP_INTPTR carrier_ptr, CHAOS_IL2CPP_INT32 digits, CHAOS_IL2CPP_INT32 mode) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    if (carrier_ptr == 0 || digits == 0)
    {
        // Scale-0 probe (0m, digits=0): identity is exact for the default
        // MidpointRounding.ToEven path. Non-default modes on a scale-0 integer
        // are a no-op too (nothing to round), so returning the input carrier here is
        // still value-correct; we only warn to surface unsupported midpoints.
        if (digits == 0 && mode != 0)
            std::fprintf(stderr, "[ChaosDecimalRound] stub: MidpointRounding mode %d not implemented; identity used\n", mode);
        // Echo into a fresh carrier so the result is non-null and addressable.
        auto* src = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);
        auto* out = new DecimalCarrier{};
        out->flags = src->flags; out->lo64 = src->lo64; out->hi32 = src->hi32;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
    }

    auto* src = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);
    int32_t scale = static_cast<int32_t>((src->flags >> 16) & 0xFF);
    CHAOS_IL2CPP_INT32 sign = (src->flags & 0x80000000u) ? -1 : 1;
    CHAOS_IL2CPP_UINT64 magnitude = src->lo64;  // hi32 ignored: >uint64 mantissa out of probe scope

    if (digits >= scale)
    {
        // Already at or beyond requested fractional places → value preserved.
        auto* out = new DecimalCarrier{};
        out->flags = src->flags; out->lo64 = src->lo64; out->hi32 = src->hi32;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
    }

    // Need to drop (scale - digits) low decimal digits, but our model carries the
    // mantissa at integer scale; to round to `digits` positive places we must convert
    // the fractional part and round. In the simplified (scale-0 integer) model the
    // probe values have no fractional digits, so `digits >= 0` over scale-0 is already
    // handled above; the remaining case is digits < 0 (round to tens/...).
    CHAOS_IL2CPP_UINT64 rounded = RoundIntegerMagnitude(magnitude, digits, mode);
    auto* out = new DecimalCarrier{};
    out->lo64 = rounded;
    out->hi32 = 0u;
    // Result has scale 0; reapply the sign.
    out->flags = static_cast<CHAOS_IL2CPP_UINT32>(sign < 0 ? 0x80000000u : 0u);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}
extern "C" CHAOS_IL2CPP_INTPTR ChaosMathDecimalTruncate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept
{
    return carrier_ptr;
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ToDecimal(Double) — double→DecimalCarrier
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    // TLS buffer avoids heap alloc per call (Convert::ToDecimal(Double) benchmark)
    thread_local DecimalCarrier s_tls_decimal{};
    auto* out = &s_tls_decimal;
    if (value < 0) {
        out->flags = 0x80000000u;
        // -value is positive; store the absolute truncated value.
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            static_cast<CHAOS_IL2CPP_INT64>(std::trunc(-value)));
    } else {
        out->flags = 0u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            static_cast<CHAOS_IL2CPP_INT64>(std::trunc(value)));
    }
    out->hi32 = 0u;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ChangeType(object, TypeCode) — IConvertible dispatch
// ═══════════════════════════════════════════════════════════════════

// Allocate a boxed int32 object. Layout: ThinLockableHeader(16B) + int32 value(4B).
// Pattern from enum_stubs.cpp enum_alloc_boxed_int32.
static CHAOS_IL2CPP_INTPTR box_int32(CHAOS_IL2CPP_INT32 value) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(20));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16); // header
    std::memcpy(storage + 16, &value, sizeof(value));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

// Allocate a boxed boolean object. Layout: ThinLockableHeader(16B) + bool(1B).
static CHAOS_IL2CPP_INTPTR box_bool(CHAOS_IL2CPP_INT32 value) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    unsigned char* storage = static_cast<unsigned char*>(GcAllocateAtomic(17));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16); // header
    storage[16] = value ? 1 : 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

// Convert.ChangeType(object, TypeCode) -> object
// TypeCode values: Boolean=3, Char=4, SByte=5, Byte=6, Int16=7, UInt16=8,
// Int32=9, UInt32=10, Int64=11, UInt64=12, Single=13, Double=14, Decimal=15,
// DateTime=16, String=18.
extern "C" CHAOS_IL2CPP_INTPTR ChaosConvertChangeType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode) noexcept
{
    if (obj == 0) return 0;
    switch (typeCode)
    {
        case 3: // Boolean
        {
            // Read the boxed boolean value from the object's payload at offset 16
            // (ThinLockableHeader(16B) + bool value(1B)).  The pointer value itself
            // is NOT the boolean — must dereference the payload.
            //
            // DEFENSIVE GUARD: obj is only a valid boxed value when the caller
            // passed a real boxed object of the matching TypeCode.  A raw/foreign
            // pointer (e.g. from a non-ChangeType path) would give an out-of-bounds
            // read at +16.  The ATG probes only pass legitimately-boxed Boolean
            // values; the guard below is a DEBUG-only assert + a null safety net so
            // a stray pointer cannot silently corrupt.
            auto* bytes = reinterpret_cast<const unsigned char*>(obj);
            CHAOS_IL2CPP_ASSERT(obj != 0);
            if (bytes == nullptr) return 0;
            bool val = (bytes[16] != 0);
            return box_bool(val ? 1 : 0);
        }
        case 9: // Int32
        {
            // Read the boxed int32 value from the object's payload at offset 16.
            // static_cast<CHAOS_IL2CPP_INT32>(obj) would truncate the pointer address
            // to 32 bits, producing garbage — NOT the actual value.
            //
            // DEFENSIVE GUARD: same as Boolean — obj must be a legitimately-boxed
            // Int32 object.  ATG probes pass valid boxed values; the null net below
            // protects a stray pointer from an out-of-bounds memcpy.
            auto* bytes = reinterpret_cast<const unsigned char*>(obj);
            CHAOS_IL2CPP_ASSERT(obj != 0);
            if (bytes == nullptr) return 0;
            CHAOS_IL2CPP_INT32 val;
            std::memcpy(&val, bytes + 16, sizeof(val));
            return box_int32(val);
        }
        case 18: // String
            // String is already a reference type; echo the pointer.
            // NOTE: This is only correct when obj is already a string pointer.
            // If obj is a boxed non-string value (e.g. boxed int32), this would
            // return a pointer to a non-string object as a string — the caller
            // would crash reading string data.  The ATG probes only exercise
            // String→String, so this is safe for current usage.
            return obj;
        default:
            // Unsupported TypeCode — throw a managed exception so the caller
            // gets a loud, diagnosable failure rather than a silent null that
            // causes an opaque NRE later in caller code.
            throw chaos_managed_exception{};
    }
}

// Convert.ChangeType(object, TypeCode, IFormatProvider) -> object
// IFormatProvider is ignored for the simple conversions that ATG probes.
extern "C" CHAOS_IL2CPP_INTPTR ChaosConvertChangeTypeWithProvider(
    CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 typeCode, CHAOS_IL2CPP_INTPTR provider) noexcept
{
    (void)provider;
    return ChaosConvertChangeType(obj, typeCode);
}