// ABI exports: extern "C" linkage for managed/NativeAOT callability.

#include "parse_convert.h"
#include "runtime_core.h"
#include "exception_helpers.h"
#include "string_table.h"
#include "generated_code_compat.h"
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>

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
// Convert::ToInt32(Double)
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INT32 ChaosConvertToInt32FromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    if (std::isnan(value) ||
        value < static_cast<double>(std::numeric_limits<CHAOS_IL2CPP_INT32>::min()) ||
        value > static_cast<double>(std::numeric_limits<CHAOS_IL2CPP_INT32>::max()))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for an Int32.");
    }
    return static_cast<CHAOS_IL2CPP_INT32>(std::trunc(value));
}

// ═══════════════════════════════════════════════════════════════════
// Convert::ToString(Int32) / ToString(Double)
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR ChaosFormatInt32(CHAOS_IL2CPP_INT32 value) noexcept
{
    char buf[16];
    int n = std::snprintf(buf, sizeof(buf), "%d", static_cast<int>(value));
    if (n < 0) return 0;
    auto id = chaos::il2cpp::string_table::Intern(buf, static_cast<CHAOS_IL2CPP_UINT32>(n));
    return chaos_make_string_id_value(id);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosFormatDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    char buf[64];
    int n = std::snprintf(buf, sizeof(buf), "%.15g", static_cast<double>(value));
    if (n < 0) return 0;
    auto id = chaos::il2cpp::string_table::Intern(buf, static_cast<CHAOS_IL2CPP_UINT32>(n));
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

// ═══════════════════════════════════════════════════════════════════
// Convert::ToDecimal(Double) — double→DecimalCarrier
// ═══════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR ChaosDecimalFromDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* out = new DecimalCarrier{};
    if (value < 0) {
        out->flags = 0x80000000u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            -static_cast<CHAOS_IL2CPP_INT64>(std::trunc(-value)));
    } else {
        out->flags = 0u;
        out->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(
            static_cast<CHAOS_IL2CPP_INT64>(std::trunc(value)));
    }
    out->hi32 = 0u;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(out);
}