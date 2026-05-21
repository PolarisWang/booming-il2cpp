// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// misc_stubs.cpp — Miscellaneous stub implementations
// Array, Buffer, Type marshalling, Culture, GC, Environment, Console, Delegate
#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/string_stubs.h"
#include "gc_helpers.h"
#include "gc/gc_old_gen.h"
#include "gc/gc_region.h"
#include "gc/gc_scheduler.h"
#include "gc/gc_young_collector.h"
#include "gc/gc_loh.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Array operations ──
void ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (array == 0 || count <= 0) return;
    auto* arr = get_managed_array_mut(array);
    if (index < 0 || count < 0) return;
    auto uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    auto ucount = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (uindex > static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) || ucount > (static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) - uindex)) return;
    if (arr->elements == nullptr) return;

    std::memset(
        reinterpret_cast<CHAOS_IL2CPP_UINT8*>(arr->elements) + uindex * sizeof(void*),
        0, ucount * sizeof(void*));
}

CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept
{
    if (array == 0) return 0;
    (void)dimension;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
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

CHAOS_IL2CPP_INT64 ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

// ── Buffer ──
CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
}

// ── Culture helpers (stub: return static non-null pointers) ──
static constexpr CHAOS_IL2CPP_SIZE kStubObjectSize = 128;
static CHAOS_IL2CPP_UINT8 s_stub_culture[kStubObjectSize]{};
static CHAOS_IL2CPP_UINT8 s_stub_subsystem[kStubObjectSize]{};
static CHAOS_IL2CPP_UINT8 s_stub_invariant_name[sizeof(StubStringHeader) + 10]{};  // "invariant\0"
static CHAOS_IL2CPP_UINT8 s_stub_invariant_display[sizeof(StubStringHeader) + 36]{};  // "Invariant Language (Invariant Country)\0"
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

CHAOS_IL2CPP_INT32 ChaosCompareInfoCompare(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept
{
    (void)compare_info;
    // Ordinal string comparison via delegated ChaosStringCompare
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosStringCompare(a, b));
}

CHAOS_IL2CPP_INT32 ChaosCompareInfoIndexOf(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept
{
    (void)compare_info;
    // Ordinal substring search via delegated ChaosStringIndexOf
    return ChaosStringIndexOf(source, value);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetTextInfo(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR /*text_info*/, CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c >= 0x41 && c <= 0x5A) return static_cast<CHAOS_IL2CPP_INTPTR>(c + 0x20);
    return static_cast<CHAOS_IL2CPP_INTPTR>(c);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR /*text_info*/, CHAOS_IL2CPP_INT32 c) noexcept
{
    if (c >= 0x61 && c <= 0x7A) return static_cast<CHAOS_IL2CPP_INTPTR>(c - 0x20);
    return static_cast<CHAOS_IL2CPP_INTPTR>(c);
}

CHAOS_IL2CPP_INTPTR ChaosTextInfoGetCultureName(CHAOS_IL2CPP_INTPTR /*text_info*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_empty_string);
}

// ── GC stubs ──
}  // extern "C"

void ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept
{
    // Synchronous blocking collection:
    //   generation < 0 or gen==2 → full collection (all generations)
    //   gen==0 → young collection only
    //   gen==1 → full collection (young + old)
    //
    // 1. Young collection on the calling thread's TLS nursery (if any).
    // 2. Full old-gen mark-sweep (STW safepoint) when gen >= 1.
    // 3. Run pending finalizers.

    // Step 1: Young collection on the shared young generation (if any).
    Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
    if (young_region != nullptr && young_region->current > young_region->begin) {
        uint32_t gen = chaos::il2cpp::runtime_core::threading::RequestGlobalSafepoint();
        GcYoungCollection();
        chaos::il2cpp::runtime_core::threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 2: Full collection when gen >= 1 or default (-1).
    if (generation < 0 || generation >= 1) {
        uint32_t gen = chaos::il2cpp::runtime_core::threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        chaos::il2cpp::runtime_core::threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 3: Run pending finalizers.
    g_old_gen.RunFinalizers();
}

extern "C" {
CHAOS_IL2CPP_INT32 ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    if (obj == 0) return 0;
    void* ptr = reinterpret_cast<void*>(obj);
    if (RegionManager::Instance().IsNurseryPointer(ptr)) return 0;
    if (g_old_gen.IsInOldGen(ptr)) return 2;
    if (g_loh.IsInLOH(ptr)) return 2;
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

CHAOS_IL2CPP_INTPTR ChaosConsoleGetError(void) noexcept { return 0; }

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


void chaos_gc_suppress_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept {
    if (obj == 0) return;
    chaos::il2cpp::runtime_core::g_old_gen.SuppressFinalizer(reinterpret_cast<void*>(obj));
}

void chaos_gc_reregister_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept {
    if (obj == 0) return;
    chaos::il2cpp::runtime_core::g_old_gen.ReRegisterFinalizer(reinterpret_cast<void*>(obj));
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core