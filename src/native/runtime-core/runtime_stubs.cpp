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
    auto* guid = static_cast<CHAOS_IL2CPP_UINT8*>(std::malloc(16));
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

    // Allocate result: header + data + NUL.
    auto* result = static_cast<StubStringHeader*>(
        std::malloc(sizeof(StubStringHeader) + total + 1));
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

// -- Global assert failure counter (incremented by assertion helpers in generated code) --
extern "C" CHAOS_IL2CPP_INT32 __chaos_assert_failures = 0;

// -- Generic registration callback (defined by generated code via static init) --
extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

// -- NameIndex module registration callback (set by generated code via static init) --
// Called at the end of BootstrapRuntime() to register per-module name index
// data for hot-patch dispatch.  Each AOT module emits a static initializer
// that sets this pointer before main().
extern "C" void (*g_chaos_register_name_index_modules)(void) = nullptr;

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

// ── InterpreterEntryDirect: extern "C" forwarding wrapper ───────────────
// The generated code emits an extern "C" forward declaration and references
// &InterpreterEntryDirect in dispatch table entries.  This wrapper provides
// the C-linkage symbol for verification builds.  The real implementation
// (in interpreter_entry.cpp) requires the chaos_interpreter library; this
// stub is sufficient for L2 checksum verification which never invokes
// hot-patch dispatch.
extern "C" void InterpreterEntryDirect(
    uintptr_t /*method_key*/,
    void*     /*args_buf*/,
    void*     /*ret_buf*/) noexcept
{
    // Not reached during L2 checksum verification.
    // Full implementation requires chaos_interpreter.lib.
}
