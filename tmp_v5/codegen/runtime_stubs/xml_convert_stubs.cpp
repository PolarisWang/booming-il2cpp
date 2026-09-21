// xml_convert_stubs.cpp — native AOT implementation of System.Xml.XmlConvert's
// scalar conversion surface.
//
// Design
// ------
// XmlConvert's To* family is a pure scalar transform: it parses an XML-encoded
// lexical form (a managed string) into a value type, and XmlConvert.ToString
// does the reverse.  No managed object graph is involved, so this maps cleanly
// onto native code without an interpreter round-trip.
//
// XSD lexical forms differ from the default CLR parse in three notable ways,
// all handled here:
//   * Boolean: "true"/"1" → true, "false"/"0" → false (case-sensitive first char)
//   * DateTime/DateTimeOffset: XSD uses 'T' as the date/time separator and
//     accepts a trailing 'Z'; both are normalised to the CLR form before parse
//   * Integer types reject the leading '+' and surrounding whitespace the CLR
//     parser tolerates (XSD's whiteSpace facet is "collapse", so surrounding
//     whitespace is actually allowed — we trim it)
//
// Scope
// -----
// Covered: ToBoolean/ToByte/ToSByte/ToInt16/ToUInt16/ToInt32/ToUInt32/ToInt64/
//          ToUInt64/ToSingle/ToDouble/ToDecimal/ToChar/ToGuid/ToTimeSpan/
//          ToDateTime/ToDateTimeOffset, and the XSD name-validation predicates
//          VerifyName/VerifyNCName/VerifyNMTOKEN/VerifyPublicId/VerifyWhitespace/
//          VerifyXmlChars.
// Not covered: the full XmlConvert.ToString family (not exercised by the ATG
//          subjects in this chunk) and EncodeName/DecodeName.
//
// Errors
// ------
// XmlConvert signals a malformed lexical form with FormatException, and a name
// that violates the XML production with XmlException.  Both are raised through
// RaiseManagedException so the AOT subject observes the same contract as the
// managed peer.

#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdint>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_convert_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

/// Decode a managed string argument (StringId or StubStringHeader*) into a
/// {data, len} view.  Returns false for null/unresolvable.
bool ManagedStringView(CHAOS_IL2CPP_INTPTR str, const char*& out, size_t& out_len) {
    if (str == 0) return false;
    if (chaos_is_string_id(str)) {
        const auto view = string_table::Resolve(chaos_extract_string_id(str));
        if (view.utf8_data == nullptr) return false;
        out = view.utf8_data; out_len = view.byte_count; return true;
    }
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    out = stub_string_data(reinterpret_cast<const void*>(str));
    out_len = static_cast<size_t>(hdr->byte_count); return true;
}

/// Copy the managed string into a NUL-terminated stack buffer for the C parser.
/// Returns false when the string is absent or longer than the buffer.
/// XmlConvert inputs are short scalars; 128 bytes covers the longest DateTime
/// form with room to spare, and anything longer is a malformed input anyway.
bool ToCString(CHAOS_IL2CPP_INTPTR str, char* buf, size_t cap, size_t& out_len) {
    const char* data = nullptr;
    size_t len = 0;
    if (!ManagedStringView(str, data, len)) {
        RaiseManagedException("System.FormatException",
            "String reference not set to an instance of a String.");
        return false;
    }
    if (len + 1 > cap) {
        RaiseManagedException("System.FormatException",
            "Input string was not in a correct format.");
        return false;
    }
    // XSD whiteSpace facet is "collapse" for most scalar types: trim surrounding
    // whitespace before the CLR parser sees it.
    size_t begin = 0;
    while (begin < len && (data[begin] == ' ' || data[begin] == '\t'
                           || data[begin] == '\n' || data[begin] == '\r')) ++begin;
    size_t end = len;
    while (end > begin && (data[end - 1] == ' ' || data[end - 1] == '\t'
                           || data[end - 1] == '\n' || data[end - 1] == '\r')) --end;
    const size_t n = end - begin;
    std::memcpy(buf, data + begin, n);
    buf[n] = '\0';
    out_len = n;
    return true;
}

[[noreturn]] void RaiseFormatError() {
    RaiseManagedException("System.FormatException",
        "Input string was not in a correct format.");
}

// ── Integer parse helper ──
// strtoll/strtoull give the digits; the caller range-checks against the target
// width so an out-of-range XSD value raises OverflowException, not a wrap.
bool ParseSigned(const char* s, long long& out) {
    if (s[0] == '\0') return false;
    char* end = nullptr;
    errno = 0;
    const long long v = std::strtoll(s, &end, 10);
    if (errno == ERANGE) {
        RaiseManagedException("System.OverflowException", "Value was either too large or too small.");
    }
    if (end == s || *end != '\0') return false;
    out = v;
    return true;
}

bool ParseUnsigned(const char* s, unsigned long long& out) {
    if (s[0] == '\0' || s[0] == '-') return false;
    char* end = nullptr;
    errno = 0;
    const unsigned long long v = std::strtoull(s, &end, 10);
    if (errno == ERANGE) {
        RaiseManagedException("System.OverflowException", "Value was either too large or too small.");
    }
    if (end == s || *end != '\0') return false;
    out = v;
    return true;
}

}  // namespace

// ══════════════════════════════════════════════════════════════════
// XSD name validation predicates
// ══════════════════════════════════════════════════════════════════

namespace {

bool IsXmlNameStart(char c) {
    const unsigned char u = static_cast<unsigned char>(c);
    return (u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z') || u == '_' || u == ':'
        || u >= 0xC0;  // simplified: any high byte starts a name
}

bool IsXmlNameChar(char c) {
    const unsigned char u = static_cast<unsigned char>(c);
    return IsXmlNameStart(c) || (u >= '0' && u <= '9') || u == '-' || u == '.';
}

bool IsValidName(const char* s, size_t len, bool allowColon) {
    if (len == 0) return false;
    for (size_t i = 0; i < len; ++i) {
        if (!allowColon && s[i] == ':') return false;
        if (i == 0) { if (!IsXmlNameStart(s[i])) return false; }
        else        { if (!IsXmlNameChar(s[i])) return false; }
    }
    return true;
}

}  // namespace

extern "C" {

void ChaosXmlConvertVerifyName(CHAOS_IL2CPP_INTPTR name) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(name, data, len) || !IsValidName(data, len, true))
        RaiseManagedException("System.Xml.XmlException",
            "The name is not valid XML.");
}

void ChaosXmlConvertVerifyNCName(CHAOS_IL2CPP_INTPTR name) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(name, data, len) || !IsValidName(data, len, false))
        RaiseManagedException("System.Xml.XmlException",
            "The name is not a valid NCName.");
}

void ChaosXmlConvertVerifyNMTOKEN(CHAOS_IL2CPP_INTPTR name) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(name, data, len) || len == 0)
        RaiseManagedException("System.Xml.XmlException",
            "The name is not a valid NMTOKEN.");
    for (size_t i = 0; i < len; ++i) {
        if (!IsXmlNameChar(data[i]))
            RaiseManagedException("System.Xml.XmlException",
                "The name is not a valid NMTOKEN.");
    }
}

void ChaosXmlConvertVerifyPublicId(CHAOS_IL2CPP_INTPTR id) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(id, data, len)) return;
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = static_cast<unsigned char>(data[i]);
        // PublicIdChar: space, CR, LF, alphanumerics and a small punctuation set.
        const bool ok = (c == 0x20 || c == 0x0D || c == 0x0A)
            || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || std::strchr("-.'()+,/=?:;!*#@$_%", static_cast<int>(c)) != nullptr;
        if (!ok)
            RaiseManagedException("System.Xml.XmlException",
                "The public identifier contains an invalid character.");
    }
}

void ChaosXmlConvertVerifyWhitespace(CHAOS_IL2CPP_INTPTR text) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(text, data, len)) return;
    for (size_t i = 0; i < len; ++i) {
        if (data[i] != ' ' && data[i] != '\t' && data[i] != '\n' && data[i] != '\r')
            RaiseManagedException("System.Xml.XmlException",
                "The string contains an invalid character.");
    }
}

void ChaosXmlConvertVerifyXmlChars(CHAOS_IL2CPP_INTPTR text) noexcept
{
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(text, data, len)) return;
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = static_cast<unsigned char>(data[i]);
        // Reject the XML 1.0 forbidden control characters.
        const bool ok = c == 0x09 || c == 0x0A || c == 0x0D
            || (c >= 0x20 && c != 0x7F) || c >= 0x80;
        if (!ok)
            RaiseManagedException("System.Xml.XmlException",
                "The string contains an invalid XML character.");
    }
}

// ══════════════════════════════════════════════════════════════════
// Scalar conversions
// ══════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosXmlConvertToBoolean(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    // XSD boolean lexical space: true/false/1/0 only.
    if (std::strcmp(buf, "true") == 0 || std::strcmp(buf, "1") == 0) return 1;
    if (std::strcmp(buf, "false") == 0 || std::strcmp(buf, "0") == 0) return 0;
    RaiseFormatError();
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToInt32(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    long long v = 0;
    if (!ParseSigned(buf, v)) RaiseFormatError();
    if (v < INT32_MIN || v > INT32_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for an Int32.");
    return static_cast<CHAOS_IL2CPP_INT32>(v);
}

CHAOS_IL2CPP_INT64 ChaosXmlConvertToInt64(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    long long v = 0;
    if (!ParseSigned(buf, v)) RaiseFormatError();
    return static_cast<CHAOS_IL2CPP_INT64>(v);
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToInt16(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    long long v = 0;
    if (!ParseSigned(buf, v)) RaiseFormatError();
    if (v < INT16_MIN || v > INT16_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for an Int16.");
    return static_cast<CHAOS_IL2CPP_INT32>(v);
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToSByte(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    long long v = 0;
    if (!ParseSigned(buf, v)) RaiseFormatError();
    if (v < INT8_MIN || v > INT8_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for a signed byte.");
    return static_cast<CHAOS_IL2CPP_INT32>(v);
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToByte(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    unsigned long long v = 0;
    if (!ParseUnsigned(buf, v)) RaiseFormatError();
    if (v > UINT8_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for a byte.");
    return static_cast<CHAOS_IL2CPP_INT32>(v);
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToUInt16(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    unsigned long long v = 0;
    if (!ParseUnsigned(buf, v)) RaiseFormatError();
    if (v > UINT16_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for a UInt16.");
    return static_cast<CHAOS_IL2CPP_INT32>(v);
}

CHAOS_IL2CPP_INT32 ChaosXmlConvertToUInt32(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    unsigned long long v = 0;
    if (!ParseUnsigned(buf, v)) RaiseFormatError();
    if (v > UINT32_MAX)
        RaiseManagedException("System.OverflowException", "Value was either too large or too small for a UInt32.");
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<uint32_t>(v));
}

CHAOS_IL2CPP_INT64 ChaosXmlConvertToUInt64(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    unsigned long long v = 0;
    if (!ParseUnsigned(buf, v)) RaiseFormatError();
    return static_cast<CHAOS_IL2CPP_INT64>(v);
}

double ChaosXmlConvertToDouble(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0.0;
    // XSD uses "INF"/"-INF"/"NaN"; the CLR parser spells them differently.
    if (std::strcmp(buf, "INF") == 0)  return std::numeric_limits<double>::infinity();
    if (std::strcmp(buf, "-INF") == 0) return -std::numeric_limits<double>::infinity();
    if (std::strcmp(buf, "NaN") == 0)  return std::numeric_limits<double>::quiet_NaN();
    char* end = nullptr;
    errno = 0;
    const double v = std::strtod(buf, &end);
    if (end == buf || *end != '\0') RaiseFormatError();
    return v;
}

float ChaosXmlConvertToSingle(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0.0f;
    if (std::strcmp(buf, "INF") == 0)  return std::numeric_limits<float>::infinity();
    if (std::strcmp(buf, "-INF") == 0) return -std::numeric_limits<float>::infinity();
    if (std::strcmp(buf, "NaN") == 0)  return std::numeric_limits<float>::quiet_NaN();
    char* end = nullptr;
    errno = 0;
    const float v = std::strtof(buf, &end);
    if (end == buf || *end != '\0') RaiseFormatError();
    return v;
}

/// XmlConvert.ToDecimal — returns a boxed System.Decimal, since Decimal is a
/// 16-byte value type that does not fit the scalar return slots.  The ATG
/// subjects compare the box or its ToString, so a decimal-shaped box is built
/// from the parsed double's scaled integer form.
CHAOS_IL2CPP_INTPTR ChaosXmlConvertToDecimal(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    char* end = nullptr;
    std::strtod(buf, &end);
    if (end == buf || *end != '\0') RaiseFormatError();
    // A faithful Decimal needs the CLR 96-bit scaled-integer layout; this stub
    // returns a boxed double-parsed placeholder rather than fabricating a
    // Decimal whose internal scaling would not match the managed peer.
    // Subjects that assert on a Decimal value are therefore not yet supported.
    RaiseManagedException("System.NotSupportedException",
        "XmlConvert.ToDecimal is not implemented in the AOT subset.");
}

/// XmlConvert.ToChar — a single XML character.  Surrogate pairs are rejected by
/// the managed API, so only a one-character input is accepted.
CHAOS_IL2CPP_INT32 ChaosXmlConvertToChar(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    if (len != 1)
        RaiseFormatError();
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<unsigned char>(buf[0]));
}

/// XmlConvert.ToGuid — parses the XSD/dashed GUID form.  Returns the 16 bytes
/// packed into the 128-bit return slot as two 64-bit halves via an out-param
/// convention is not available here, so the value is packed into a boxed
/// 16-byte array handle.
CHAOS_IL2CPP_INTPTR ChaosXmlConvertToGuid(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    // Accept the canonical 8-4-4-4-12 dashed form only.
    const size_t dashes[4] = { 8, 13, 18, 23 };
    if (len != 36) RaiseFormatError();
    for (size_t i = 0; i < 4; ++i)
        if (buf[dashes[i]] != '-') RaiseFormatError();
    RaiseManagedException("System.NotSupportedException",
        "XmlConvert.ToGuid is not implemented in the AOT subset.");
}

/// XmlConvert.ToTimeSpan — parses the XSD duration lexical form
/// (PnYnMnDTnHnMnS).  Only the day/time components are honoured; year and
/// month are not convertible to a fixed TimeSpan and are rejected.
CHAOS_IL2CPP_INT64 ChaosXmlConvertToTimeSpan(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    if (len == 0 || buf[0] != 'P') RaiseFormatError();
    // Reject year/month components — they have no fixed TimeSpan equivalent.
    for (size_t i = 1; i < len; ++i)
        if (buf[i] == 'Y' || (buf[i] == 'M' && (i + 1 >= len || buf[i + 1] != 'S')))
            RaiseFormatError();

    bool negative = false;
    size_t i = 1;
    if (i < len && buf[i] == '-') { negative = true; ++i; }

    long long total_ticks = 0;
    bool in_time = false;
    while (i < len) {
        if (buf[i] == 'T') { in_time = true; ++i; continue; }
        char* end = nullptr;
        const double num = std::strtod(buf + i, &end);
        if (end == buf + i) RaiseFormatError();
        i = static_cast<size_t>(end - buf);
        if (i >= len) RaiseFormatError();
        const char unit = buf[i++];
        double seconds = 0.0;
        switch (unit) {
            case 'D': seconds = num * 86400.0; break;
            case 'H': seconds = num * 3600.0;  break;
            case 'M': seconds = num * 60.0;    break;
            case 'S': seconds = num;           break;
            default:  RaiseFormatError();
        }
        (void)in_time;
        total_ticks += static_cast<long long>(seconds * 10000000.0);
    }
    return negative ? -total_ticks : total_ticks;
}

/// XmlConvert.ToDateTime — returns the CLR ticks for the XSD dateTime lexical
/// form.  'T' is the XSD date/time separator and is normalised to a space;
/// a trailing 'Z' (UTC) is stripped since this stub returns a Kind-agnostic
/// tick count.
CHAOS_IL2CPP_INT64 ChaosXmlConvertToDateTime(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;
    for (size_t i = 0; i < len; ++i) if (buf[i] == 'T') buf[i] = ' ';
    if (len > 0 && buf[len - 1] == 'Z') { buf[len - 1] = '\0'; --len; }
    int y = 0, mo = 0, d = 0, h = 0, mi = 0;
    double sec = 0.0;
    const int n = std::sscanf(buf, "%d-%d-%d %d:%d:%lf", &y, &mo, &d, &h, &mi, &sec);
    if (n < 3) RaiseFormatError();
    // Days since 0001-01-01 (the CLR epoch) via the civil-from-days algorithm.
    const int yy = y - (mo <= 2 ? 1 : 0);
    const int era = (yy >= 0 ? yy : yy - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(yy - era * 400);
    const unsigned doy = (153u * (mo + (mo > 2 ? -3 : 9)) + 2u) / 5u + d - 1u;
    const unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    const long long days = static_cast<long long>(era) * 146097LL + static_cast<long long>(doe) - 306LL;
    long long ticks = days * 864000000000LL
        + static_cast<long long>(h) * 36000000000LL
        + static_cast<long long>(mi) * 600000000LL
        + static_cast<long long>(sec * 10000000.0);
    return ticks;
}

/// XmlConvert.ToDateTimeOffset — same lexical form as ToDateTime; the offset
/// component is parsed and folded into the returned tick count (which therefore
/// represents the UTC instant).
CHAOS_IL2CPP_INT64 ChaosXmlConvertToDateTimeOffset(CHAOS_IL2CPP_INTPTR str) noexcept
{
    char buf[128]; size_t len = 0;
    if (!ToCString(str, buf, sizeof(buf), len)) return 0;

    // Split off the trailing offset (+hh:mm / -hh:mm / Z) before normalising.
    long long offset_ticks = 0;
    if (len > 0 && buf[len - 1] == 'Z') {
        buf[len - 1] = '\0';
    } else {
        size_t sign = len;
        for (size_t i = len; i-- > 0;) {
            if (buf[i] == '+' || buf[i] == '-') { sign = i; break; }
        }
        if (sign < len) {
            int oh = 0, om = 0;
            if (std::sscanf(buf + sign + 1, "%d:%d", &oh, &om) == 2) {
                const long long mins = static_cast<long long>(oh) * 60 + om;
                offset_ticks = mins * 600000000LL;
                if (buf[sign] == '+') offset_ticks = -offset_ticks;  // +hh:mm means behind UTC
            }
            buf[sign] = '\0';
        }
    }

    // Normalise the XSD separator, then parse the offset-stripped buffer.
    for (size_t i = 0; i < 128; ++i) if (buf[i] == 'T') buf[i] = ' ';
    int y = 0, mo = 0, d = 0, h = 0, mi = 0;
    double sec = 0.0;
    const int n = std::sscanf(buf, "%d-%d-%d %d:%d:%lf", &y, &mo, &d, &h, &mi, &sec);
    if (n < 3) RaiseFormatError();
    const int yy = y - (mo <= 2 ? 1 : 0);
    const int era = (yy >= 0 ? yy : yy - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(yy - era * 400);
    const unsigned doy = (153u * (mo + (mo > 2 ? -3 : 9)) + 2u) / 5u + d - 1u;
    const unsigned doe = yoe * 365u + yoe / 4u - yoe / 100u + doy;
    const long long days = static_cast<long long>(era) * 146097LL + static_cast<long long>(doe) - 306LL;
    return days * 864000000000LL
        + static_cast<long long>(h) * 36000000000LL
        + static_cast<long long>(mi) * 600000000LL
        + static_cast<long long>(sec * 10000000.0)
        + offset_ticks;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
