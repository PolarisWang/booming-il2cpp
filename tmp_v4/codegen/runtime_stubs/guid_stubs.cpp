// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// guid_stubs.cpp — Guid stub implementations (real, not stubs)
#include <chaos/native_types.h>
#include <chaos/pal/pal_random.h>
#include <cstdio>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Hex nibble extraction (scalar fallback) ──────────────────────────
static CHAOS_IL2CPP_UINT8 HexNibble(char c) noexcept
{
    if (c >= '0' && c <= '9') return static_cast<CHAOS_IL2CPP_UINT8>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'A' + 10);
    return 0xFF;
}

// ── SSE4.1 hex-to-nibble converter (x64 only) ────────────────────────
// Converts 16 ASCII hex characters to 16 nibble values in parallel.
// Returns (nibbles, valid_mask) where valid_mask has 0xFF for valid hex digits.
#if defined(_M_AMD64) || defined(__x86_64__)
#include <smmintrin.h>  // SSE4.1 for _mm_blendv_epi8

struct SimdHexResult {
    __m128i nibbles;  // 0-15 for valid hex values
    __m128i valid;    // 0xFF per byte for valid, 0x00 for invalid
};

static SimdHexResult simd_hex_to_nibble(__m128i chunk) noexcept {
    // Digit path: c - '0', valid if result <= 9
    __m128i sub0 = _mm_sub_epi8(chunk, _mm_set1_epi8('0'));
    __m128i is_digit = _mm_cmplt_epi8(sub0, _mm_set1_epi8(10));

    // Letter path: lowercase, subtract 'a', valid if result < 6
    __m128i lower = _mm_or_si128(chunk, _mm_set1_epi8(0x20));
    __m128i sub_a = _mm_sub_epi8(lower, _mm_set1_epi8('a'));
    __m128i is_letter = _mm_cmplt_epi8(sub_a, _mm_set1_epi8(6));

    // Combined validity mask
    __m128i valid = _mm_or_si128(is_digit, is_letter);

    // Letter value = sub_a + 10
    __m128i letter_val = _mm_add_epi8(sub_a, _mm_set1_epi8(10));

    // Select: digit uses sub0, letter uses letter_val
    __m128i nibbles = _mm_blendv_epi8(letter_val, sub0, is_digit);

    return { nibbles, valid };
}
#endif // x86_64

// ── NEON hex-to-nibble converter (ARM64) ────────────────────────────
#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>

struct SimdHexResult {
    uint8x16_t nibbles;
    uint8x16_t valid;
};

static int movemask_neon(uint8x16_t input) noexcept {
    // Emulate _mm_movemask_epi8: extract MSB of each byte into 16-bit mask.
    // Each byte contributes its own power-of-2 weight via the bitmask,
    // so pairwise-accumulate is equivalent to bitwise OR.
    static const uint8_t kBitMask[16] = {1,2,4,8,16,32,64,128,1,2,4,8,16,32,64,128};
    uint8x16_t t = vandq_u8(vshrq_n_u8(input, 7), vld1q_u8(kBitMask));
    uint32x4_t p = vpaddlq_u16(vpaddlq_u8(t));
    uint32x2_t r = vpadd_u32(vget_low_u32(p), vget_high_u32(p));
    return static_cast<int>(vget_lane_u32(r, 0) | (vget_lane_u32(r, 1) << 8));
}

static SimdHexResult simd_hex_to_nibble(uint8x16_t chunk) noexcept {
    // Digit path: c - '0', valid if result <= 9
    uint8x16_t sub0 = vsubq_u8(chunk, vdupq_n_u8('0'));
    uint8x16_t is_digit = vcltq_s8(vreinterpretq_s8_u8(sub0), vdupq_n_s8(10));

    // Letter path: lowercase, subtract 'a', valid if result < 6
    uint8x16_t lower = vorrq_u8(chunk, vdupq_n_u8(0x20));
    uint8x16_t sub_a = vsubq_u8(lower, vdupq_n_u8('a'));
    uint8x16_t is_letter = vcltq_s8(vreinterpretq_s8_u8(sub_a), vdupq_n_s8(6));

    // Combined validity mask
    uint8x16_t valid = vorrq_u8(is_digit, is_letter);

    // Letter value = sub_a + 10
    uint8x16_t letter_val = vaddq_u8(sub_a, vdupq_n_u8(10));

    // Select: digit uses sub0, letter uses letter_val
    // vbslq_u8(mask, a, b): mask bit=1 -> a, 0 -> b.
    // _mm_blendv_epi8(a, b, mask): mask MSB=1 -> b, 0 -> a.
    // So vbslq_u8(is_digit, sub0, letter_val) matches blendv(letter_val, sub0, is_digit).
    uint8x16_t nibbles = vbslq_u8(is_digit, sub0, letter_val);

    return { nibbles, valid };
}
#endif // ARM_NEON

// ── Scalable GUID string parser (SIMD + scalar fallback) ────────────
// Decodes a "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" format string from
// `value` (StringId or CHAOS_IL2CPP_STRING_TYPE*) into 16 raw bytes.
// Returns true on success, false on invalid input.
static bool parse_guid_string(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_UINT8 out_guid[16]) noexcept
{
    if (value == 0) return false;

    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;

    if (chaos_is_string_id(value))
    {
        auto view = string_table::Resolve(chaos_extract_string_id(value));
        data = view.utf8_data;
        len = static_cast<CHAOS_IL2CPP_INT32>(view.byte_count);
    }
    else
    {
        auto ms = reinterpret_cast<const CHAOS_IL2CPP_STRING_TYPE*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(value));
        len = ms->length;
        if (len > 0) data = ms->utf8_data;
    }
    if (data == nullptr || len < 36) return false;
    if (data[8] != '-' || data[13] != '-' || data[18] != '-' || data[23] != '-') return false;

#if defined(_M_AMD64) || defined(__x86_64__)
    // SIMD fast path: convert 36 bytes (3×16-byte chunks) to nibbles in parallel
    __m128i c0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data));
    __m128i c1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + 16));
    __m128i c2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + 32));

    auto r0 = simd_hex_to_nibble(c0);
    auto r1 = simd_hex_to_nibble(c1);
    auto r2 = simd_hex_to_nibble(c2);

    // Validate all 36 bytes (first 32 in c0+c1, last 4 in c2 masked by leading 0x00)
    // Only check positions that are actually hex chars (not hyphens at 8,13,18,23)
    __m128i all_valid = _mm_and_si128(r0.valid, r1.valid);
    // c2[0..3] must all be valid; positions 32,33,34,35 are all hex chars
    int valid_mask = _mm_movemask_epi8(all_valid);
    if ((valid_mask & 0xB7FF) != 0xB7FF) return false;  // mask out hyphen positions 8,13
    int valid2 = _mm_movemask_epi8(r2.valid);
    if ((valid2 & 0x0F) != 0x0F) return false;

    // Store nibbles and pack adjacent pairs into bytes
    alignas(16) uint8_t n0[16], n1[16], n2[16];
    _mm_store_si128(reinterpret_cast<__m128i*>(n0), r0.nibbles);
    _mm_store_si128(reinterpret_cast<__m128i*>(n1), r1.nibbles);
    _mm_store_si128(reinterpret_cast<__m128i*>(n2), r2.nibbles);

    // Pack adjacent nibble pairs into bytes using kPos table
    // kPos[i] = first hex char position for GUID byte i
    // kPos[i]+1 = second hex char position
    static constexpr int kPos[16] = {0,2,4,6, 9,11, 14,16, 19,21, 24,26,28,30,32,34};
    for (int i = 0; i < 16; i++) {
        int p = kPos[i];
        uint8_t hi = (p < 16) ? n0[p] : ((p < 32) ? n1[p - 16] : n2[p - 32]);
        uint8_t lo = (p + 1 < 16) ? n0[p + 1] : ((p + 1 < 32) ? n1[p + 1 - 16] : n2[p + 1 - 32]);
        out_guid[i] = static_cast<CHAOS_IL2CPP_UINT8>((hi << 4) | lo);
    }
    return true;
#elif defined(__ARM_NEON) || defined(__aarch64__)
    // NEON SIMD fast path: same algorithm as x86 SSE4.1 path
    uint8x16_t c0 = vld1q_u8(reinterpret_cast<const uint8_t*>(data));
    uint8x16_t c1 = vld1q_u8(reinterpret_cast<const uint8_t*>(data + 16));
    uint8x16_t c2 = vld1q_u8(reinterpret_cast<const uint8_t*>(data + 32));

    auto r0 = simd_hex_to_nibble(c0);
    auto r1 = simd_hex_to_nibble(c1);
    auto r2 = simd_hex_to_nibble(c2);

    uint8x16_t all_valid = vandq_u8(r0.valid, r1.valid);
    int valid_mask = movemask_neon(all_valid);
    if ((valid_mask & 0xB7FF) != 0xB7FF) return false;
    int valid2 = movemask_neon(r2.valid);
    if ((valid2 & 0x0F) != 0x0F) return false;

    alignas(16) uint8_t n0[16], n1[16], n2[16];
    vst1q_u8(n0, r0.nibbles);
    vst1q_u8(n1, r1.nibbles);
    vst1q_u8(n2, r2.nibbles);

    static constexpr int kPos[16] = {0,2,4,6, 9,11, 14,16, 19,21, 24,26,28,30,32,34};
    for (int i = 0; i < 16; i++) {
        int p = kPos[i];
        uint8_t hi = (p < 16) ? n0[p] : ((p < 32) ? n1[p - 16] : n2[p - 32]);
        uint8_t lo = (p + 1 < 16) ? n0[p + 1] : ((p + 1 < 32) ? n1[p + 1 - 16] : n2[p + 1 - 32]);
        out_guid[i] = static_cast<CHAOS_IL2CPP_UINT8>((hi << 4) | lo);
    }
    return true;
#else
    // Scalar fallback for non-x64 platforms
    static constexpr int kPos[16] = {0,2,4,6, 9,11, 14,16, 19,21, 24,26,28,30,32,34};
    for (int i = 0; i < 16; i++)
    {
        int p = kPos[i];
        auto hi = HexNibble(data[p]);
        auto lo = HexNibble(data[p + 1]);
        if (hi == 0xFF || lo == 0xFF) return false;
        out_guid[i] = static_cast<CHAOS_IL2CPP_UINT8>((hi << 4) | lo);
    }
    return true;
#endif
}

// ── Managed string allocator ──────────────────────────────────────────
// Allocates a chaos_managed_string (40B header + inline data) so the
// generated code's CHAOS_IL2CPP_STRING_TYPE* path reads length/utf8_data
// at the correct offsets. Avoids string_table::Intern overhead.
static CHAOS_IL2CPP_INTPTR alloc_guid_string(const char* utf8, CHAOS_IL2CPP_UINT32 byte_count) noexcept
{
    constexpr CHAOS_IL2CPP_SIZE kHeaderSize = 40;
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(kHeaderSize + byte_count + 1));
    if (storage == nullptr) return 0;

    std::memset(storage, 0, kHeaderSize);

    // length at offset 16 (after ThinLockableHeader)
    auto* len_field = reinterpret_cast<CHAOS_IL2CPP_INT32*>(storage + 16);
    *len_field = static_cast<CHAOS_IL2CPP_INT32>(byte_count);

    // inline data, utf8_data pointer at offset 24
    char* data_area = reinterpret_cast<char*>(storage + kHeaderSize);
    if (byte_count > 0) std::memcpy(data_area, utf8, byte_count);
    data_area[byte_count] = '\0';
    auto* utf8_field = reinterpret_cast<const char**>(storage + 24);
    *utf8_field = data_area;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

void ChaosGuidCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (instance == 0) return;
    // Decode the GUID string into the Guid struct at instance+16
    // (after ThinLockableHeader = 16 bytes)
    CHAOS_IL2CPP_UINT8 raw_guid[16];
    if (parse_guid_string(value, raw_guid)) {
        std::memcpy(reinterpret_cast<void*>(instance + 16), raw_guid, 16);
    } else {
        std::memset(reinterpret_cast<void*>(instance + 16), 0, 16);
    }
}

CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept
{
    // Thread-local buffer avoids GcAllocateAtomic(16) per call.
    // Valid until the next ChaosGuidNewGuid call on the same thread.
    thread_local CHAOS_IL2CPP_UINT8 s_guid_buf[16];

    if (!chaos::il2cpp::pal::PalRandomBytes(s_guid_buf, 16)) {
        std::memset(s_guid_buf, 0, 16);
    }

    s_guid_buf[6] = (s_guid_buf[6] & 0x0F) | 0x40;
    s_guid_buf[8] = (s_guid_buf[8] & 0x3F) | 0x80;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_guid_buf);
}

CHAOS_IL2CPP_INTPTR ChaosGuidParse(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_UINT8 raw_guid[16];
    if (!parse_guid_string(value, raw_guid)) return 0;

    auto* guid = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(16));
    if (guid == nullptr) return 0;
    std::memcpy(guid, raw_guid, 16);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guid);
}

CHAOS_IL2CPP_INT32 ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid) noexcept
{
    if (guid == 0) return 0;
    const auto* bytes = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(guid);

    auto read_i32_le = [](const CHAOS_IL2CPP_UINT8* b) noexcept -> int32_t {
        return static_cast<int32_t>(b[0]) | (static_cast<int32_t>(b[1]) << 8)
             | (static_cast<int32_t>(b[2]) << 16) | (static_cast<int32_t>(b[3]) << 24);
    };
    auto read_i16_le = [](const CHAOS_IL2CPP_UINT8* b) noexcept -> int16_t {
        return static_cast<int16_t>(b[0]) | (static_cast<int16_t>(b[1]) << 8);
    };

    const int32_t _a = read_i32_le(bytes);
    const int16_t _b = read_i16_le(bytes + 4);
    const int16_t _c = read_i16_le(bytes + 6);

    int32_t result = _a;
    result ^= static_cast<int32_t>((static_cast<uint16_t>(_b) << 16) | static_cast<uint16_t>(_c));
    result ^= static_cast<int32_t>((static_cast<uint32_t>(bytes[8]) << 24)
                                  | (static_cast<uint32_t>(bytes[9]) << 16)
                                  | (static_cast<uint32_t>(bytes[10]) << 8)
                                  | bytes[11]);
    result ^= static_cast<int32_t>((static_cast<uint32_t>(bytes[12]) << 24)
                                  | (static_cast<uint32_t>(bytes[13]) << 16)
                                  | (static_cast<uint32_t>(bytes[14]) << 8)
                                  | bytes[15]);
    return result;
}

CHAOS_IL2CPP_INTPTR ChaosGuidToString(CHAOS_IL2CPP_INTPTR guid) noexcept
{
    if (guid == 0) return 0;
    const auto* bytes = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(guid);

    // Fast hex LUT -- no snprintf format-string parsing
    static constexpr char kHex[] = "0123456789abcdef";
    char buf[37];
    auto put_hex = [&](int& p, CHAOS_IL2CPP_UINT8 b) noexcept {
        buf[p++] = kHex[b >> 4];
        buf[p++] = kHex[b & 0x0F];
    };
    int pos = 0;
    put_hex(pos, bytes[0]); put_hex(pos, bytes[1]);
    put_hex(pos, bytes[2]); put_hex(pos, bytes[3]);
    buf[pos++] = '-';
    put_hex(pos, bytes[4]); put_hex(pos, bytes[5]);
    buf[pos++] = '-';
    put_hex(pos, bytes[6]); put_hex(pos, bytes[7]);
    buf[pos++] = '-';
    put_hex(pos, bytes[8]); put_hex(pos, bytes[9]);
    buf[pos++] = '-';
    put_hex(pos, bytes[10]); put_hex(pos, bytes[11]);
    put_hex(pos, bytes[12]); put_hex(pos, bytes[13]);
    put_hex(pos, bytes[14]); put_hex(pos, bytes[15]);
    buf[pos] = '\0';

    // Allocate a proper chaos_managed_string with inline data.
    // Avoids string_table::Intern (lock + hash + insert) since GUID
    // strings are unique per call and never match existing entries.
    return alloc_guid_string(buf, 36);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
