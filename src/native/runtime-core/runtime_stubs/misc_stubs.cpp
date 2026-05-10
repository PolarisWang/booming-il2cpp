// misc_stubs.cpp — Miscellaneous stub implementations
// Array, Buffer, Type marshalling, Culture, GC, Environment, Console, Delegate
#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Array operations ──
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
    return static_cast<CHAOS_IL2CPP_INT32>(hdr->length);
}

// ── Type marshalling helpers ──
CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
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

// ── Buffer ──
CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return 0;
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(array);
    return static_cast<CHAOS_IL2CPP_INT32>(hdr->length);
}

// ── Culture helpers (stub: return static non-null pointers) ──
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

// ── GC stubs ──
void ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept
{
    (void)generation;
}

CHAOS_IL2CPP_INT32 ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    (void)obj;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosGcGetMaxGeneration(void) noexcept
{
    return 2;
}

// ── Environment / Console ──
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept
{
    return 0;
}

void ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept
{
    (void)value;
}

// ── Delegate ──
void ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept
{
    (void)delegate_obj; (void)target; (void)method_ptr;
}

CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept
{
    (void)delegate_obj;
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core