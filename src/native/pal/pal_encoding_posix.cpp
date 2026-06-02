// pal_encoding_posix.cpp — POSIX string encoding implementation

#include <chaos/pal/pal_encoding.h>

namespace chaos::il2cpp::pal {

int32_t PalUtf8ToUtf16(const uint8_t* utf8, int32_t utf8_len,
                       uint16_t* utf16, int32_t utf16_capacity) noexcept {
    if (utf8 == nullptr || utf8_len < 0) return -1;

    // Phase 1: count required UTF-16 capacity.
    int needed = 0;
    const auto* in = utf8;
    const auto* end = in + utf8_len;
    while (in < end) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) { ++needed; }
        else { needed += 2; }
    }
    if (utf16 == nullptr || utf16_capacity <= 0) return needed;

    // Phase 2: write UTF-16 output.
    int written = 0;
    in = utf8;
    while (in < end && written < utf16_capacity) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) {
            utf16[written++] = static_cast<uint16_t>(cp);
        } else if (written + 1 < utf16_capacity) {
            cp -= 0x10000;
            utf16[written++] = static_cast<uint16_t>(0xD800 | (cp >> 10));
            utf16[written++] = static_cast<uint16_t>(0xDC00 | (cp & 0x3FF));
        }
    }
    return written;
}

int32_t PalUtf16ToUtf8(const uint16_t* utf16, int32_t utf16_len,
                       uint8_t* utf8, int32_t utf8_capacity) noexcept {
    if (utf16 == nullptr || utf16_len < 0) return -1;

    // Phase 1: count required UTF-8 capacity.
    int needed = 0;
    for (int i = 0; i < utf16_len; ++i) {
        uint32_t cp = utf16[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < utf16_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (utf16[++i] - 0xDC00);
        }
        needed += (cp < 0x80) ? 1 : (cp < 0x800) ? 2 : (cp < 0x10000) ? 3 : 4;
    }
    if (utf8 == nullptr || utf8_capacity <= 0) return needed;

    // Phase 2: write UTF-8 output.
    int pos = 0;
    for (int i = 0; i < utf16_len && pos < utf8_capacity; ++i) {
        uint32_t cp = utf16[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < utf16_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (utf16[++i] - 0xDC00);
        }
        if (cp < 0x80) {
            utf8[pos++] = static_cast<uint8_t>(cp);
        } else if (cp < 0x800) {
            if (pos + 1 >= utf8_capacity) break;
            utf8[pos++] = static_cast<uint8_t>(0xC0 | (cp >> 6));
            utf8[pos++] = static_cast<uint8_t>(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            if (pos + 2 >= utf8_capacity) break;
            utf8[pos++] = static_cast<uint8_t>(0xE0 | (cp >> 12));
            utf8[pos++] = static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F));
            utf8[pos++] = static_cast<uint8_t>(0x80 | (cp & 0x3F));
        } else {
            if (pos + 3 >= utf8_capacity) break;
            utf8[pos++] = static_cast<uint8_t>(0xF0 | (cp >> 18));
            utf8[pos++] = static_cast<uint8_t>(0x80 | ((cp >> 12) & 0x3F));
            utf8[pos++] = static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F));
            utf8[pos++] = static_cast<uint8_t>(0x80 | (cp & 0x3F));
        }
    }
    return pos;
}

bool PalCloseHandle(intptr_t /*handle*/) noexcept {
    return true;  // POSIX: no opaque handle to close.
}

}  // namespace chaos::il2cpp::pal
