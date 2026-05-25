// json_reader.h — Minimal single-header JSON parser for AotCoreIr deserialization.
//
// Handles the subset of JSON produced by System.Text.Json:
//   - objects: {"key": value, ...}
//   - arrays:  [value, ...]
//   - strings: "..." (with basic escape sequences)
//   - numbers: integer and floating-point (parsed as int64_t or double)
//   - null, true, false
//
// Not a general-purpose JSON parser. No UTF-16 surrogates, no Unicode
// escape (\uXXXX) support. Single-pass, no allocation aside from output values.

#ifndef CHAOS_IL2CPP_JSON_READER_H_
#define CHAOS_IL2CPP_JSON_READER_H_

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <chaos/native_types.h>

namespace chaos::il2cpp::json {

// ── Result types ──────────────────────────────────────────────────────────

enum class JsonValueKind {
    Null,
    Bool,
    Int64,
    Double,
    String,
    Array,
    Object,
};

struct JsonValue {
    JsonValueKind kind = JsonValueKind::Null;
    union {
        bool      bool_value;
        int64_t   int64_value;
        double    double_value;
    };
    const char* string_value = nullptr;
    size_t      string_length = 0;

    // Points into the source buffer: start of the array/object token.
    const char* data_start = nullptr;
    size_t      data_length = 0;

    // True when string_value was heap-allocated (escape decoding).
    // Caller must call JsonFreeString() to release.
    bool        owns_string = false;

    // Convenience accessors
    bool IsNull()    const { return kind == JsonValueKind::Null; }
    bool IsBool()    const { return kind == JsonValueKind::Bool; }
    bool IsInt64()   const { return kind == JsonValueKind::Int64; }
    bool IsDouble()  const { return kind == JsonValueKind::Double; }
    bool IsString()  const { return kind == JsonValueKind::String; }
    bool IsArray()   const { return kind == JsonValueKind::Array; }
    bool IsObject()  const { return kind == JsonValueKind::Object; }

    bool   AsBool()   const { return bool_value; }
    int64_t AsInt64() const { return int64_value; }
    double AsDouble() const { return double_value; }
    const char* AsCString() const { return string_value; }
};

// ── Parse result ──────────────────────────────────────────────────────────

struct ParseResult {
    JsonValue value;
    const char* error = nullptr;  // null on success
};

// ── Parser state ──────────────────────────────────────────────────────────

struct JsonParser {
    const char* pos;
    const char* end;

    JsonParser(const char* begin, const char* end)
        : pos(begin), end(end) {}

    // Skip whitespace.
    void SkipWhitespace() {
        while (pos < end) {
            char c = *pos;
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                ++pos;
            } else {
                break;
            }
        }
    }

    // Peek current character (after skipping whitespace).
    char Peek() {
        SkipWhitespace();
        return (pos < end) ? *pos : '\0';
    }

    // Consume a single character (after skipping whitespace).
    // Returns false if the character doesn't match.
    bool Expect(char c) {
        SkipWhitespace();
        if (pos < end && *pos == c) {
            ++pos;
            return true;
        }
        return false;
    }

    // Parse a string value.
    ParseResult ParseString() {
        ParseResult result;
        SkipWhitespace();
        if (pos >= end || *pos != '"') {
            result.error = "expected '\"' for string";
            return result;
        }

        // Find the closing quote, handling basic escapes.
        const char* start = pos + 1;
        size_t len = 0;
        bool has_escape = false;

        const char* scan = start;
        while (scan < end) {
            if (*scan == '\\') {
                has_escape = true;
                scan += 2;  // skip escape sequence
                len += 1;
                continue;
            }
            if (*scan == '"') {
                break;
            }
            ++scan;
            ++len;
        }
        if (scan >= end) {
            result.error = "unterminated string";
            return result;
        }

        const char* str_start = start;
        size_t str_len = len;

        if (has_escape) {
            // Allocate and unescape.  Uses CHAOS_IL2CPP_MALLOC (raw domain).
            auto* buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(len + 1));
            if (!buf) { result.error = "out of memory"; return result; }
            const char* src = start;
            char* dst = buf;
            while (src < scan) {
                if (*src == '\\') {
                    switch (src[1]) {
                        case '"':  *dst++ = '"';  break;
                        case '\\': *dst++ = '\\'; break;
                        case '/':  *dst++ = '/';  break;
                        case 'n':  *dst++ = '\n'; break;
                        case 'r':  *dst++ = '\r'; break;
                        case 't':  *dst++ = '\t'; break;
                        default:   *dst++ = src[1]; break;
                    }
                    src += 2;
                } else {
                    *dst++ = *src++;
                }
            }
            *dst = '\0';
            result.value.kind = JsonValueKind::String;
            result.value.string_value = buf;
            result.value.string_length = len;
            result.value.owns_string = true;  // caller must free
        } else {
            result.value.kind = JsonValueKind::String;
            result.value.string_value = str_start;
            result.value.string_length = len;
        }

        // Advance past closing quote.
        pos = scan + 1;
        return result;
    }

    // Parse a number (integer or floating-point).
    ParseResult ParseNumber() {
        ParseResult result;
        SkipWhitespace();
        const char* start = pos;
        bool is_float = false;

        if (pos < end && (*pos == '-' || *pos == '+')) ++pos;
        while (pos < end && *pos >= '0' && *pos <= '9') ++pos;
        if (pos < end && *pos == '.') { is_float = true; ++pos; }
        while (pos < end && *pos >= '0' && *pos <= '9') ++pos;
        if (pos < end && (*pos == 'e' || *pos == 'E')) {
            is_float = true; ++pos;
            if (pos < end && (*pos == '+' || *pos == '-')) ++pos;
            while (pos < end && *pos >= '0' && *pos <= '9') ++pos;
        }

        // Extract the token.
        constexpr size_t kBufSize = 64;
        char buf[kBufSize];
        size_t token_len = static_cast<size_t>(pos - start);
        if (token_len >= kBufSize) { result.error = "number too long"; return result; }
        std::memcpy(buf, start, token_len);
        buf[token_len] = '\0';

        if (is_float) {
            char* endp = nullptr;
            double val = std::strtod(buf, &endp);
            if (endp != buf + token_len) { result.error = "invalid float"; return result; }
            result.value.kind = JsonValueKind::Double;
            result.value.double_value = val;
        } else {
            char* endp = nullptr;
            int64_t val = std::strtoll(buf, &endp, 10);
            if (endp != buf + token_len) { result.error = "invalid integer"; return result; }
            result.value.kind = JsonValueKind::Int64;
            result.value.int64_value = val;
        }
        return result;
    }

    // Parse a literal (null, true, false).
    ParseResult ParseLiteral() {
        ParseResult result;
        SkipWhitespace();
        if (pos + 4 <= end && std::memcmp(pos, "null", 4) == 0) {
            result.value.kind = JsonValueKind::Null;
            pos += 4;
        } else if (pos + 4 <= end && std::memcmp(pos, "true", 4) == 0) {
            result.value.kind = JsonValueKind::Bool;
            result.value.bool_value = true;
            pos += 4;
        } else if (pos + 5 <= end && std::memcmp(pos, "false", 5) == 0) {
            result.value.kind = JsonValueKind::Bool;
            result.value.bool_value = false;
            pos += 5;
        } else {
            result.error = "expected null, true, or false";
        }
        return result;
    }

    // Parse a full JSON value (object, array, string, number, or literal).
    ParseResult ParseValue() {
        ParseResult result;
        SkipWhitespace();
        if (pos >= end) { result.error = "unexpected end of input"; return result; }

        char c = *pos;
        if (c == '{') return ParseObject();
        if (c == '[') return ParseArray();
        if (c == '"') return ParseString();
        if (c == '-' || (c >= '0' && c <= '9')) return ParseNumber();
        return ParseLiteral();
    }

    // Parse a JSON object: {"key": value, ...}
    ParseResult ParseObject() {
        ParseResult result;
        SkipWhitespace();
        if (pos >= end || *pos != '{') { result.error = "expected '{'"; return result; }
        const char* start = pos;
        ++pos;  // skip '{'

        result.value.kind = JsonValueKind::Object;
        result.value.data_start = start;

        // Scan to matching '}', tracking nesting.
        int depth = 1;
        bool in_string = false;
        while (pos < end && depth > 0) {
            char c = *pos;
            if (in_string) {
                if (c == '\\') { pos += 2; continue; }
                if (c == '"') in_string = false;
            } else {
                if (c == '"') in_string = true;
                else if (c == '{') ++depth;
                else if (c == '}') --depth;
            }
            ++pos;
        }
        if (depth != 0) { result.error = "unterminated object"; return result; }

        result.value.data_length = static_cast<size_t>(pos - start);
        return result;
    }

    // Parse a JSON array: [value, ...]
    ParseResult ParseArray() {
        ParseResult result;
        SkipWhitespace();
        if (pos >= end || *pos != '[') { result.error = "expected '['"; return result; }
        const char* start = pos;
        ++pos;  // skip '['

        result.value.kind = JsonValueKind::Array;
        result.value.data_start = start;

        // Scan to matching ']', tracking nesting.
        int depth = 1;
        bool in_string = false;
        while (pos < end && depth > 0) {
            char c = *pos;
            if (in_string) {
                if (c == '\\') { pos += 2; continue; }
                if (c == '"') in_string = false;
            } else {
                if (c == '"') in_string = true;
                else if (c == '[') ++depth;
                else if (c == ']') --depth;
            }
            ++pos;
        }
        if (depth != 0) { result.error = "unterminated array"; return result; }

        result.value.data_length = static_cast<size_t>(pos - start);
        return result;
    }

    // ── Convenience: find a key in an object ─────────────────────────
    // Given a JsonValue of kind Object, find the value for a key.
    // Returns a null JsonValue if not found.
    static JsonValue FindKey(const JsonValue& obj, const char* key) {
        JsonValue not_found;
        if (obj.kind != JsonValueKind::Object) return not_found;

        JsonParser p(obj.data_start + 1, obj.data_start + obj.data_length - 1);
        while (true) {
            p.SkipWhitespace();
            if (p.pos >= p.end) break;

            // Parse key string.
            auto key_result = p.ParseString();
            if (key_result.error) break;

            // Compare key.
            size_t key_len = std::strlen(key);
            bool match = (key_result.value.string_length == key_len &&
                         std::memcmp(key_result.value.string_value, key, key_len) == 0);

            // Expect ':'
            if (!p.Expect(':')) break;

            if (match) {
                return p.ParseValue().value;
            } else {
                // Skip value.
                auto skip = p.ParseValue();
                if (skip.error) break;
            }

            // Expect ',' or end.
            p.SkipWhitespace();
            if (p.pos < p.end && *p.pos == ',') {
                ++p.pos;
            }
        }
        return not_found;
    }

    // ── Convenience: iterate array elements ──────────────────────────
    // Call `callback(index, value)` for each element. Returns false if
    // the input is not an array.
    template<typename Fn>
    static bool ForEachArrayElement(const JsonValue& arr, Fn callback) {
        if (arr.kind != JsonValueKind::Array) return false;

        JsonParser p(arr.data_start + 1, arr.data_start + arr.data_length - 1);
        size_t index = 0;
        bool first = true;

        while (true) {
            p.SkipWhitespace();
            if (p.pos >= p.end) break;

            if (!first) {
                if (*p.pos != ',') break;
                ++p.pos;
                p.SkipWhitespace();
            }
            first = false;

            auto elem = p.ParseValue();
            if (elem.error) break;

            if (!callback(index, elem.value)) break;
            ++index;
        }
        return true;
    }

    // ── Top-level parse ──────────────────────────────────────────────
    static ParseResult Parse(const char* json, size_t length) {
        JsonParser parser(json, json + length);
        auto result = parser.ParseValue();
        // Check for trailing non-whitespace.
        parser.SkipWhitespace();
        if (result.error == nullptr && parser.pos < parser.end) {
            // Trailing content is acceptable for our subset (we slice objects/arrays).
        }
        return result;
    }
};

// ── Free allocated string data ────────────────────────────────────────────
// Call this on a JsonValue to release any heap-allocated string data
// that was allocated during escape-sequence decoding.
inline void JsonFreeString(const JsonValue& val) {
    if (val.kind == JsonValueKind::String && val.owns_string) {
        CHAOS_IL2CPP_FREE(const_cast<char*>(val.string_value));
    }
}

}  // namespace chaos::il2cpp::json

#endif  // CHAOS_IL2CPP_JSON_READER_H_