// runtime_stubs.cpp — Pre-existing runtime helper stubs
//
// These functions are referenced by generated C++ code but have no real
// runtime implementation yet. All stubs return 0 / nullptr / no-op so
// that native compilation succeeds. Replace with real implementations
// as the corresponding managed features are brought online.
//
// See generated_code_compat.h for the extern "C" declarations.

#include <chaos/native_types.h>
#include <chaos/trace.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <cstdio>
#include <time.h>
#endif

#include "runtime_instantiation.h"
#include "runtime_abi.h"
#include "interpreter_entry.h"
#include "generated_code_compat.h"
#include "string_table.h"
#include "gc_helpers.h"  // GcAllocate

// Object header layouts (mirrors runtime_core.cpp)
struct StubArrayHeader {
    CHAOS_IL2CPP_INTPTR element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

struct StubStringHeader {
    CHAOS_IL2CPP_INTPTR type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

// Helper: get UTF-8 data pointer from a managed string.
static const char* stub_string_data(const void* str) noexcept {
    return reinterpret_cast<const char*>(static_cast<const StubStringHeader*>(str) + 1);
}

namespace chaos::il2cpp::runtime_core {
extern "C"
{

// ── Array operations ───────────────────────────────────────────────
// Array layout: ArrayHeader(16) + elements.
// Element size defaults to pointer-width (works for reference types; value-type
// arrays need per-element-size which requires TypeInfo* resolution — deferred).

void ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (array == 0 || count <= 0) return;
    auto* hdr = reinterpret_cast<StubArrayHeader*>(array);
    if (index < 0 || count < 0) return;
    auto uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    auto ucount = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (uindex > hdr->length || ucount > (hdr->length - uindex)) return;

    auto* elements = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(hdr + 1);
    std::memset(elements + uindex * sizeof(void*), 0, ucount * sizeof(void*));
}

CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept
{
    if (array == 0) return 0;
    (void)dimension;
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(array);
    // For SZArray (single-dimension, zero-based), dimension is always 0.
    return static_cast<CHAOS_IL2CPP_INT32>(hdr->length);
}

// ── Type marshalling helpers ───────────────────────────────────────
// The eval stack stores values as CHAOS_IL2CPP_INTPTR. These helpers
// pack/unpack 64-bit integer and double values. On x64, CHAOS_IL2CPP_INTPTR
// is already 64 bits so store/load_int64 are identity operations.

CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_INT64 ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// ─── Buffer operations ─────────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return 0;
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(array);
    // byte[] array: byte_count == element count == length.
    return static_cast<CHAOS_IL2CPP_INT32>(hdr->length);
}

// ─── DateTime operations ──────────────────────────────────────────
// .NET DateTime stores ticks (100-ns intervals since 0001-01-01) in
// a 64-bit value. Lower 62 bits = ticks, upper 2 bits = DateTimeKind.

// Offset from .NET epoch (0001-01-01) to FILETIME epoch (1601-01-01).
static constexpr CHAOS_IL2CPP_INT64 kDotNetToFileTimeOffset = 504911232000000000LL;
// Offset from .NET epoch to Unix epoch (1970-01-01).
static constexpr CHAOS_IL2CPP_INT64 kDotNetToUnixEpochOffset = 621355968000000000LL;

CHAOS_IL2CPP_INT64 ChaosDatetimeGetUtcNow(void) noexcept
{
#if defined(_WIN32)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    CHAOS_IL2CPP_INT64 filetime = (static_cast<CHAOS_IL2CPP_INT64>(ft.dwHighDateTime) << 32)
                                | static_cast<CHAOS_IL2CPP_INT64>(ft.dwLowDateTime);
    // FILETIME is 100-ns intervals since 1601-01-01; add offset to get .NET ticks.
    return filetime + kDotNetToFileTimeOffset;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    CHAOS_IL2CPP_INT64 ticks = static_cast<CHAOS_IL2CPP_INT64>(ts.tv_sec) * 10000000LL
                             + ts.tv_nsec / 100;
    return ticks + kDotNetToUnixEpochOffset;
#endif
}

CHAOS_IL2CPP_INT32 ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept
{
    if (datetime == 0) return 0;
    // DateTime stores ticks in lower 62 bits; upper 2 bits = DateTimeKind.
    CHAOS_IL2CPP_INT64 date_data;
    std::memcpy(&date_data, reinterpret_cast<const void*>(datetime), sizeof(date_data));
    CHAOS_IL2CPP_INT64 ticks = date_data & 0x3FFFFFFFFFFFFFFFLL;
    return static_cast<CHAOS_IL2CPP_INT32>(ticks)
         ^ static_cast<CHAOS_IL2CPP_INT32>(ticks >> 32);
}

// ─── Math operations ──────────────────────────────────────────────

CHAOS_IL2CPP_INT64 ChaosMathSqrt(CHAOS_IL2CPP_INT64 value) noexcept
{
    double d;
    CHAOS_IL2CPP_MEMCPY(&d, &value, sizeof(d));
    d = CHAOS_IL2CPP_SQRT(d);
    CHAOS_IL2CPP_INT64 result;
    CHAOS_IL2CPP_MEMCPY(&result, &d, sizeof(result));
    return result;
}

// ─── Math stubs (return 0 for methods not yet implemented) ───────
CHAOS_IL2CPP_INT32 ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathRound(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathRound2(CHAOS_IL2CPP_FLOAT64 value, CHAOS_IL2CPP_INT32 digits) noexcept { (void)value; (void)digits; return 0.0; }
CHAOS_IL2CPP_INT32 ChaosMathMaxInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathMaxDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept { (void)a; (void)b; return 0.0; }
CHAOS_IL2CPP_INT32 ChaosMathMinInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathPow(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept { (void)x; (void)y; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathSin(CHAOS_IL2CPP_FLOAT64 x) noexcept { (void)x; return 0.0; }
CHAOS_IL2CPP_FLOAT64 ChaosMathCos(CHAOS_IL2CPP_FLOAT64 x) noexcept { (void)x; return 0.0; }
CHAOS_IL2CPP_INT64 ChaosMathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept { (void)a; (void)b; return 0; }

// ─── DateTime stubs ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt) noexcept { (void)dt; return 0; }
CHAOS_IL2CPP_INTPTR ChaosDateTimeToStringFormat(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_INTPTR format) noexcept { (void)dt; (void)format; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddMinutes(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT32 ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right) noexcept { (void)left; (void)right; return 0; }
CHAOS_IL2CPP_INT32 ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month) noexcept { (void)year; (void)month; return 0; }
// ─── TimeSpan stubs ─────────────────────────────────────────────
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanParse(CHAOS_IL2CPP_INTPTR value) noexcept { (void)value; return 0; }

// ─── DateTime/TimeSpan ctor stubs ──────────────────────────────
void ChaosDateTimeCtor3(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day) noexcept { (void)instance; (void)year; (void)month; (void)day; }
void ChaosDateTimeCtor6(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day, CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute, CHAOS_IL2CPP_INT32 second) noexcept { (void)instance; (void)year; (void)month; (void)day; (void)hour; (void)minute; (void)second; }
CHAOS_IL2CPP_INTPTR ChaosDateTimeParse(CHAOS_IL2CPP_INTPTR value) noexcept { (void)value; return 0; }
void ChaosTimeSpanCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 hours, CHAOS_IL2CPP_INT32 minutes, CHAOS_IL2CPP_INT32 seconds) noexcept { (void)instance; (void)hours; (void)minutes; (void)seconds; }

// ─── Interlocked / threading ──────────────────────────────────────

void ChaosInterlockedMemoryBarrier(void) noexcept
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

// ─── Exception helpers ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    (void)exc;
    // Stub: managed Exception._innerException field offset requires
    // layout-engine resolution — deferred to Phase 5.
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    (void)exc;
    return 0;
}

// ─── Object helpers ───────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    return 0;
}

void ChaosObjectCtor(CHAOS_IL2CPP_INTPTR /*obj*/) noexcept
{
}

// ─── GUID / Random ────────────────────────────────────────────────
// Uses a thread_local xorshift32 PRNG seeded from time + address.

static uint32_t stub_xorshift32() noexcept {
    thread_local uint32_t state = 0;
    if (state == 0) {
        state = static_cast<uint32_t>(
            reinterpret_cast<uintptr_t>(&state) ^
            static_cast<uint32_t>(std::time(nullptr)));
        if (state == 0) state = 1;
    }
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}

CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept
{
    auto* guid = static_cast<CHAOS_IL2CPP_UINT8*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(16));
    if (guid == nullptr) return 0;

#if defined(_WIN32)
    GUID win_guid;
    if (CoCreateGuid(&win_guid) == S_OK) {
        std::memcpy(guid, &win_guid, 16);
    } else {
        std::memset(guid, 0, 16);
    }
#else
    // Read 16 random bytes from /dev/urandom.
    std::FILE* fp = std::fopen("/dev/urandom", "rb");
    if (fp) {
        size_t nread = std::fread(guid, 1, 16, fp);
        std::fclose(fp);
        if (nread != 16) std::memset(guid, 0, 16);
    } else {
        // Fallback: generate all 16 bytes from xorshift32.
        for (int i = 0; i < 4; ++i) {
            uint32_t v = stub_xorshift32();
            std::memcpy(guid + i * 4, &v, 4);
        }
    }
    // Set UUID v4 (random) variant bits: version=4, variant=RFC 4122.
    guid[6] = (guid[6] & 0x0F) | 0x40;
    guid[8] = (guid[8] & 0x3F) | 0x80;
#endif

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guid);
}

void ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept
{
    (void)rng;
    if (buffer == 0) return;
    auto* hdr = reinterpret_cast<StubArrayHeader*>(buffer);
    auto* bytes = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(hdr + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < hdr->length; ++i) {
        bytes[i] = static_cast<CHAOS_IL2CPP_UINT8>(stub_xorshift32());
    }
}

CHAOS_IL2CPP_INT64 ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    // Generate double in [0, 1) from 53 random bits.
    uint64_t bits = (static_cast<uint64_t>(stub_xorshift32()) << 21)
                  | (static_cast<uint64_t>(stub_xorshift32()) >> 11);
    // IEEE 754: exponent = 0x3FF (biased = 0), mantissa = 52 random bits.
    bits = (0x3FFULL << 52) | (bits & 0x000FFFFFFFFFFFFFULL);
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    // d is in [1, 2); subtract 1.0 to get [0, 1).
    d -= 1.0;
    CHAOS_IL2CPP_INT64 result;
    std::memcpy(&result, &d, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32());
}

CHAOS_IL2CPP_INT32 ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept
{
    (void)rng;
    if (maxValue <= 0) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32() % static_cast<uint32_t>(maxValue));
}

// ─── Guid helpers ──────────────────────────────────────────────

static CHAOS_IL2CPP_UINT8 HexNibble(char c) noexcept
{
    if (c >= '0' && c <= '9') return static_cast<CHAOS_IL2CPP_UINT8>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<CHAOS_IL2CPP_UINT8>(c - 'A' + 10);
    return 0xFF;
}

CHAOS_IL2CPP_INTPTR ChaosGuidParse(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;

    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;

    if (chaos_is_string_id(value))
    {
        auto view = chaos::il2cpp::string_table::Resolve(chaos_extract_string_id(value));
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
    if (data == nullptr || len < 36) return 0;
    if (data[8] != '-' || data[13] != '-' || data[18] != '-' || data[23] != '-') return 0;

    auto* guid = static_cast<CHAOS_IL2CPP_UINT8*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(16));
    if (guid == nullptr) return 0;

    static constexpr int kPos[16] = {0,2,4,6, 9,11, 14,16, 19,21, 24,26,28,30,32,34};
    for (int i = 0; i < 16; i++)
    {
        int p = kPos[i];
        auto hi = HexNibble(data[p]);
        auto lo = HexNibble(data[p + 1]);
        if (hi == 0xFF || lo == 0xFF) { return 0; }
        guid[i] = static_cast<CHAOS_IL2CPP_UINT8>((hi << 4) | lo);
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(guid);
}

CHAOS_IL2CPP_INT32 ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid) noexcept
{
    if (guid == 0) return 0;
    const auto* bytes = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(guid);

    // Match .NET Guid.GetHashCode() exactly.
    // Managed layout: _a (int32 LE, bytes 0-3), _b (int16 LE, bytes 4-5), _c (int16 LE, bytes 6-7),
    // _d.._k (8 bytes, bytes 8-15).
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
    char buf[37];
    std::snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        bytes[0], bytes[1], bytes[2], bytes[3],
        bytes[4], bytes[5], bytes[6], bytes[7],
        bytes[8], bytes[9], bytes[10], bytes[11],
        bytes[12], bytes[13], bytes[14], bytes[15]);
    auto str_id = chaos::il2cpp::string_table::Intern(buf, 36);
    return chaos_make_string_id_value(str_id);
}

// ─── HashCode helpers ──────────────────────────────────────────

static CHAOS_IL2CPP_UINT32 ChaosHashCodeRotateLeft32(CHAOS_IL2CPP_UINT32 value, int offset) noexcept {
    return (value << offset) | (value >> (32 - offset));
}

static CHAOS_IL2CPP_UINT32 ChaosHashCodeMixFinal(CHAOS_IL2CPP_UINT32 hash) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime2 = 2246822519u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    hash ^= hash >> 15;
    hash *= kPrime2;
    hash ^= hash >> 13;
    hash *= kPrime3;
    hash ^= hash >> 16;
    return hash;
}

static CHAOS_IL2CPP_UINT32 ChaosHashCodeQueueRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 queued_value) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime4 = 668265263u;
    return ChaosHashCodeRotateLeft32(hash + queued_value * kPrime3, 17) * kPrime4;
}

CHAOS_IL2CPP_INT32 ChaosHashCodeToHashCode(CHAOS_IL2CPP_INTPTR state) noexcept
{
    (void)state;
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    constexpr CHAOS_IL2CPP_UINT32 kSeed = 0u;
    CHAOS_IL2CPP_UINT32 hash = kSeed + kPrime5;
    hash += 0u;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosHashCodeMixFinal(hash));
}

CHAOS_IL2CPP_INT32 ChaosHashCodeCombine2(CHAOS_IL2CPP_INT32 hc1, CHAOS_IL2CPP_INT32 hc2) noexcept
{
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    constexpr CHAOS_IL2CPP_UINT32 kSeed = 0u;
    CHAOS_IL2CPP_UINT32 hash = kSeed + kPrime5;
    hash += 8u;
    hash = ChaosHashCodeQueueRound(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc1));
    hash = ChaosHashCodeQueueRound(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc2));
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosHashCodeMixFinal(hash));
}

void ChaosHashCodeAdd(CHAOS_IL2CPP_INTPTR state, CHAOS_IL2CPP_INT32 value) noexcept
{
    (void)state;
    (void)value;
}

// ─── Culture helpers ──────────────────────────────────────────────
// Stub: returns static non-null pointers to prevent NREs. Managed
// CultureInfo/CompareInfo/DateTimeFormatInfo/NumberFormatInfo objects
// require layout-engine field offsets — full implementation deferred.

static constexpr CHAOS_IL2CPP_SIZE kStubObjectSize = 128;
static CHAOS_IL2CPP_UINT8 s_stub_culture[kStubObjectSize]{};
static CHAOS_IL2CPP_UINT8 s_stub_subsystem[kStubObjectSize]{};
static CHAOS_IL2CPP_UINT8 s_stub_empty_string[sizeof(StubStringHeader) + 1]{};

CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

// ─── String helpers ───────────────────────────────────────────────
// Managed strings: StringObjectHeader(16) + UTF-8 bytes + NUL.

CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    if (vh->byte_count == 0) return 1;

    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* vdata = stub_string_data(reinterpret_cast<const void*>(value));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= sh->byte_count - vh->byte_count; ++i) {
        if (std::memcmp(sdata + i, vdata, vh->byte_count) == 0) return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    return (std::memcmp(stub_string_data(reinterpret_cast<const void*>(str)), stub_string_data(reinterpret_cast<const void*>(value)),
                        vh->byte_count) == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (separator == 0 || value == 0) return 0;
    auto* sep_hdr = reinterpret_cast<const StubStringHeader*>(separator);
    auto* arr_hdr = reinterpret_cast<StubArrayHeader*>(value);

    CHAOS_IL2CPP_UINTPTR count = arr_hdr->length;
    CHAOS_IL2CPP_UINTPTR sep_len = sep_hdr->byte_count;

    // Compute total byte count.
    CHAOS_IL2CPP_UINTPTR total = 0;
    CHAOS_IL2CPP_INTPTR* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<void*>(arr_hdr + 1));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem) total += elem->byte_count;
    }
    if (count > 1) total += sep_len * (count - 1);

    // Allocate result: header + data + NUL using GC_ATOMIC so managed
    // string wrappers can hold it without explicit free.
    auto* result = static_cast<StubStringHeader*>(
        chaos::il2cpp::runtime_core::GcAllocateAtomic(sizeof(StubStringHeader) + total + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = total;

    char* dest = reinterpret_cast<char*>(result + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        if (i > 0 && sep_len > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(separator)), sep_len);
            dest += sep_len;
        }
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem && elem->byte_count > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(elements[i])), elem->byte_count);
            dest += elem->byte_count;
        }
    }
    *dest = '\0';

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ─── Reflection helpers (deferred — needs ECMA metadata) ──────────

CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return (p->default_value_blob != 0 && p->default_value_size > 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return p->default_value_blob;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    // Same as GetDefaultValue — the blob IS the raw ECMA Constant value
    return ChaosReflectionGetDefaultValue(param);
}

// ─── Reflection deferred stubs ──────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    // Requires method flags in the descriptor — deferred until codegen
    // adds a flags field to ReflectionQueryMethodDescriptor.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member_handle) noexcept
{
    // For non-override methods the base definition is the method itself.
    // Override-chain resolution (walking parent vtable slots) requires
    // metadata beyond the current query model — deferred.
    return member_handle;
}

// ─── Thread helpers ──────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept
{
    return 0;
}

// ─── Runtime helpers ──────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR /*format*/, CHAOS_IL2CPP_INTPTR /*args*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept
{
    // Identity comparison — correct for reference types.
    return (left == right) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    // Identity hash: use the pointer as the hash value.
    auto p = static_cast<CHAOS_IL2CPP_UINTPTR>(value);
    return static_cast<CHAOS_IL2CPP_INT32>(p ^ (p >> 32));
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept
{
    // For reference types, returns the object as-is.
    // For boxed value types, returns a pointer to the value inside the box.
    // Stub: identity return works for reference-typed callers.
    return value;
}

// ─── Runtime-wrapped exception ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    // RuntimeWrappedException wraps a non-Exception CLR object as an Exception.
    // Stub: return the input (correct when input is already the inner value).
    return exc;
}

// ─── Float32 marshalling ───────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

// ─── Volatile operations ──────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    return *reinterpret_cast<volatile CHAOS_IL2CPP_INT32*>(ptr);
}

// -- Generic registration callback (defined by generated code via static init) --
extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

// -- Hotpatch module registration callback (set by generated code via static init) --
// Called at the end of BootstrapRuntime() to register per-module hotpatch
// data for hot-patch dispatch.  Each AOT module emits a static initializer
// that sets this pointer before main().
extern "C" void (*g_chaos_register_hotpatch_modules)(void) = nullptr;

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

// ── InterpreterEntryDirect: extern "C" forwarding wrapper ───────────────
// The generated code emits an extern "C" forward declaration and references
// &InterpreterEntryDirect in dispatch table entries.  This wrapper provides
// the C-linkage symbol for verification builds.  The real implementation
// (in interpreter_entry.cpp) requires the chaos_interpreter library; this
// stub is sufficient for Fact Static checksum verification which never invokes
// hot-patch dispatch.
extern "C" void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept
{
    // Delegate to the real implementation in interpreter_entry.cpp.
    // The generated code references &InterpreterEntryDirect (extern "C" linkage),
    // so the dispatch table entries would otherwise call the empty stub.
    chaos::il2cpp::runtime_core::InterpreterEntryDirect(method_key, args_buf, ret_buf);
}
