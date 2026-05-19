// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// guid_stubs.cpp — Guid stub implementations (real, not stubs)
#include <chaos/native_types.h>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <cstdio>
#endif

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Shared GUID string parser ────────────────────────────────────────
static CHAOS_IL2CPP_UINT8 HexNibble(char c) noexcept
{
    if (c >= '0' && c <= '9') return static_cast<CHAOS_IL2CPP_UINT8>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'A' + 10);
    return 0xFF;
}

// ── Shared GUID string parser ────────────────────────────────────────
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

#if defined(_WIN32)
    // BCryptGenRandom is a lightweight syscall, much faster than CoCreateGuid (COM).
    if (BCryptGenRandom(nullptr, s_guid_buf, 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0) {
        std::memset(s_guid_buf, 0, 16);
    }
#else
    std::FILE* fp = std::fopen("/dev/urandom", "rb");
    if (fp) {
        size_t nread = std::fread(s_guid_buf, 1, 16, fp);
        std::fclose(fp);
        if (nread != 16) std::memset(s_guid_buf, 0, 16);
    } else {
        for (int i = 0; i < 4; ++i) {
            uint32_t v = stub_xorshift32();
            std::memcpy(s_guid_buf + i * 4, &v, 4);
        }
    }
#endif
    // Set UUID version 4 (random) and variant bits per RFC 4122
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
