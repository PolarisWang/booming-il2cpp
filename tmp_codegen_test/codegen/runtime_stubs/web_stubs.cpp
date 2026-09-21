// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// web_stubs.cpp — WebUtility.HtmlEncode/Decode, UrlEncode/Decode stubs
#include <chaos/native_types.h>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Internal: is a char unreserved for URL encoding? ──
static bool is_url_unreserved(char c) noexcept {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           c == '-' || c == '.' || c == '_' || c == '~';
}

// ── Internal: hex digit for percent-encoding ──
static char to_hex(uint8_t nibble) noexcept {
    return nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
}

// ── Internal: hex digit from percent-encoded char ──
static uint8_t from_hex(char c) noexcept {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// ── Internal: resolve a managed string argument ──
static CHAOS_IL2CPP_INTPTR resolve_string_arg(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    if (!chaos_is_string_id(value))
        return value;
    return chaos_resolve_string_by_id(value);
}

// ── Internal: HTML entity table ──
struct HtmlEntity {
    const char* entity;
    uint8_t entity_len;
    char result;
};

static const HtmlEntity kHtmlEntities[] = {
    { "&amp;",   5, '&' },
    { "&lt;",    4, '<' },
    { "&gt;",    4, '>' },
    { "&quot;",  6, '"' },
    { "&#39;",   5, '\'' },
    // Named entities for special characters (subset)
    { "&nbsp;",  6, 160 },  //
    { "&iexcl;", 7, 161 },  // ¡
    { "&cent;",  6, 162 },  // ¢
    { "&pound;", 7, 163 },  // £
    { "&curren;",8, 164 },  // ¤
};

static constexpr int kHtmlEntityCount = sizeof(kHtmlEntities) / sizeof(kHtmlEntities[0]);

// ── Internal: match HTML entity at position i in data ──
static int match_entity(const char* data, CHAOS_IL2CPP_UINTPTR len,
                        CHAOS_IL2CPP_UINTPTR i, char& result) noexcept {
    if (data[i] != '&') return 0;
    // Numeric entity: &#NN; or &#xHH;
    if (i + 3 < len && data[i + 1] == '#') {
        CHAOS_IL2CPP_UINTPTR j = i + 2;
        uint32_t cp = 0;
        int base = 10;
        if (data[j] == 'x' || data[j] == 'X') { base = 16; ++j; }
        while (j < len && data[j] != ';') {
            cp *= base;
            if (data[j] >= '0' && data[j] <= '9') cp += data[j] - '0';
            else if (base == 16 && data[j] >= 'a' && data[j] <= 'f') cp += data[j] - 'a' + 10;
            else if (base == 16 && data[j] >= 'A' && data[j] <= 'F') cp += data[j] - 'A' + 10;
            else return 0;
            ++j;
        }
        if (j >= len || data[j] != ';') return 0;
        // For ASCII-range results, emit single byte
        if (cp <= 0x7F) { result = static_cast<char>(cp); return static_cast<int>(j - i + 1); }
        // For non-ASCII, approximate as '?' (caller handles single-byte replacement)
        result = '?';
        return static_cast<int>(j - i + 1);
    }
    // Named entities
    for (int e = 0; e < kHtmlEntityCount; ++e) {
        if (i + kHtmlEntities[e].entity_len <= len &&
            std::memcmp(data + i, kHtmlEntities[e].entity, kHtmlEntities[e].entity_len) == 0) {
            result = kHtmlEntities[e].result;
            return kHtmlEntities[e].entity_len;
        }
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════
// HtmlEncode: replace & < > " ' with HTML entities
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INTPTR ChaosWebUtilityHtmlEncode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    value = resolve_string_arg(value);
    if (value == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(value);
    const char* data = stub_string_data(reinterpret_cast<const void*>(value));
    CHAOS_IL2CPP_UINTPTR len = hdr->byte_count;

    // First pass: compute output length
    CHAOS_IL2CPP_UINTPTR out_len = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        switch (data[i]) {
            case '&': out_len += 5; break;   // &amp;
            case '<': out_len += 4; break;   // &lt;
            case '>': out_len += 4; break;   // &gt;
            case '"': out_len += 6; break;   // &quot;
            case '\'': out_len += 5; break;  // &#39;
            default:  out_len += 1; break;
        }
    }

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + out_len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = out_len;
    char* dst = reinterpret_cast<char*>(result + 1);

    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        switch (data[i]) {
            case '&': std::memcpy(dst, "&amp;", 5); dst += 5; break;
            case '<': std::memcpy(dst, "&lt;", 4); dst += 4; break;
            case '>': std::memcpy(dst, "&gt;", 4); dst += 4; break;
            case '"': std::memcpy(dst, "&quot;", 6); dst += 6; break;
            case '\'': std::memcpy(dst, "&#39;", 5); dst += 5; break;
            default:  *dst++ = data[i]; break;
        }
    }
    *dst = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ═══════════════════════════════════════════════════════════════
// HtmlDecode: replace HTML entities with actual chars
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INTPTR ChaosWebUtilityHtmlDecode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    value = resolve_string_arg(value);
    if (value == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(value);
    const char* data = stub_string_data(reinterpret_cast<const void*>(value));
    CHAOS_IL2CPP_UINTPTR len = hdr->byte_count;

    // First pass: compute output length
    CHAOS_IL2CPP_UINTPTR out_len = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ) {
        char decoded = 0;
        int consumed = match_entity(data, len, i, decoded);
        if (consumed > 0) {
            out_len += 1;
            i += consumed;
        } else {
            out_len += 1;
            ++i;
        }
    }

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + out_len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = out_len;
    char* dst = reinterpret_cast<char*>(result + 1);

    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ) {
        char decoded = 0;
        int consumed = match_entity(data, len, i, decoded);
        if (consumed > 0) {
            *dst++ = decoded;
            i += consumed;
        } else {
            *dst++ = data[i];
            ++i;
        }
    }
    *dst = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ═══════════════════════════════════════════════════════════════
// UrlEncode: percent-encode special characters
// ── Unreserved (a-z, A-Z, 0-9, - . _ ~) pass through.
// ── Space → '+'
// ── Everything else → %XX
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INTPTR ChaosWebUtilityUrlEncode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    value = resolve_string_arg(value);
    if (value == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(value);
    const char* data = stub_string_data(reinterpret_cast<const void*>(value));
    CHAOS_IL2CPP_UINTPTR len = hdr->byte_count;

    // First pass: compute output length
    CHAOS_IL2CPP_UINTPTR out_len = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        uint8_t c = static_cast<uint8_t>(data[i]);
        if (is_url_unreserved(static_cast<char>(c))) {
            out_len += 1;
        } else if (c == ' ') {
            out_len += 1;
        } else {
            out_len += 3;  // %XX
        }
    }

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + out_len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = out_len;
    char* dst = reinterpret_cast<char*>(result + 1);

    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        uint8_t c = static_cast<uint8_t>(data[i]);
        if (is_url_unreserved(static_cast<char>(c))) {
            *dst++ = static_cast<char>(c);
        } else if (c == ' ') {
            *dst++ = '+';
        } else {
            *dst++ = '%';
            *dst++ = to_hex(c >> 4);
            *dst++ = to_hex(c & 0x0F);
        }
    }
    *dst = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ═══════════════════════════════════════════════════════════════
// UrlDecode: reverse percent-encoding
// ── '+' → space
// ── %XX → character
// ═══════════════════════════════════════════════════════════════
CHAOS_IL2CPP_INTPTR ChaosWebUtilityUrlDecode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    value = resolve_string_arg(value);
    if (value == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(value);
    const char* data = stub_string_data(reinterpret_cast<const void*>(value));
    CHAOS_IL2CPP_UINTPTR len = hdr->byte_count;

    // First pass: compute output length
    CHAOS_IL2CPP_UINTPTR out_len = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        if (data[i] == '%' && i + 2 < len) {
            out_len += 1;
            i += 2;
        } else {
            out_len += 1;
        }
    }

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + out_len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = out_len;
    char* dst = reinterpret_cast<char*>(result + 1);

    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        if (data[i] == '+') {
            *dst++ = ' ';
        } else if (data[i] == '%' && i + 2 < len) {
            uint8_t c = (from_hex(data[i + 1]) << 4) | from_hex(data[i + 2]);
            *dst++ = static_cast<char>(c);
            i += 2;
        } else {
            *dst++ = data[i];
        }
    }
    *dst = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
