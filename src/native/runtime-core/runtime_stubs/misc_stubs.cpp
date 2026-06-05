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
#include "gc_heap.h"
#include "gc/gc_old_gen.h"
#include "gc/gc_region.h"
#include "gc/gc_scheduler.h"
#include "gc/gc_young_collector.h"
#include "gc/gc_loh.h"
#include "thread_state.h"
#include "runtime_stubs/array_stubs.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Array operations ──
void ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
{
    ::ChaosArrayClear_Inline(array, index, count);
}

CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept
{
    if (array == 0) return 0;
    (void)dimension;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
}

// ── Type marshalling helpers ──
// Implemented as CHAOS_IL2CPP_FORCEINLINE in misc_stubs.h to eliminate
// function call overhead (~10-20 cycles) for trivial identity casts.

// ── Buffer ──
CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    return static_cast<CHAOS_IL2CPP_INT32>(arr->length);
}

void ChaosBufferMemmove(CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_SIZE count) noexcept
{
    if (count > 0 && dest != 0 && src != 0)
        std::memmove(reinterpret_cast<void*>(dest), reinterpret_cast<void*>(src), count);
}

void ChaosBufferMemoryCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT64 /*dest_size*/, CHAOS_IL2CPP_INT64 src_bytes) noexcept
{
    if (src_bytes > 0 && dest != 0 && source != 0)
        std::memmove(reinterpret_cast<void*>(dest), reinterpret_cast<void*>(source), static_cast<CHAOS_IL2CPP_SIZE>(src_bytes));
}

void ChaosBufferBlockCopy(CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_INT32 src_offset, CHAOS_IL2CPP_INTPTR dst, CHAOS_IL2CPP_INT32 dst_offset, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (count <= 0 || src == 0 || dst == 0) return;
    const auto* src_arr = get_managed_array(src);
    auto* dst_arr = get_managed_array_mut(dst);
    const auto* src_data = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(src_arr));
    auto* dst_data = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dst_arr));
    std::memmove(dst_data + dst_offset, src_data + src_offset, static_cast<CHAOS_IL2CPP_SIZE>(count));
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

// ── Globalization stubs (ASCII-range) ──
// These provide fast-path native implementations for commonly-called
// globalization methods.  Only the ASCII range (0x00-0x7F) is covered;
// non-ASCII inputs return safe defaults.  Full ICU integration would
// require linking libicuuc / icuuc libraries.

// UnicodeCategory for ASCII characters (0x00-0x7F)
static constexpr CHAOS_IL2CPP_INT32 s_asciiCategory[128] = {
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  // 00-0F: Cc
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  // 10-1F: Cc
    11,25,25,25,27,28,25,25,25,25,25,25,26,25,26,25,  // 20-2F: Zs Po Po Po Sc Sk Po Po Po Po Po Po Sm Po Sm Po
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8,25,26,25,26,25,25,  // 30-3F: Nd Nd Nd Nd Nd Nd Nd Nd Nd Nd Po Sm Po Sm Po Po
    25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40-4F: Po Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,25,25,25,26,25,  // 50-5F: Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Lu Po Po Po Sm Po
    25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60-6F: Po Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,25,25,25,26,14,  // 70-7F: Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Ll Po Po Po Sm Cc
};

CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValue(CHAOS_IL2CPP_INT32 ch) noexcept
{
    // ASCII digits 0-9
    if (ch >= 0x30 && ch <= 0x39) return static_cast<CHAOS_IL2CPP_FLOAT64>(ch - 0x30);
    return -1.0;  // Not a numeric value
}

CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept
{
    if (ch >= 0x30 && ch <= 0x39) return ch - 0x30;
    return -1;
}

CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDecimalDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept
{
    if (ch >= 0x30 && ch <= 0x39) return ch - 0x30;
    return -1;
}

CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetUnicodeCategory(CHAOS_IL2CPP_INT32 ch) noexcept
{
    if (ch >= 0 && ch < 128) return s_asciiCategory[ch];
    return 30;  // OtherNotAssigned for non-ASCII
}

CHAOS_IL2CPP_INT32 ChaosCompareInfoIsSortableString(CHAOS_IL2CPP_INTPTR str) noexcept
{
    // Return 1 (true) for any non-null string
    return str != 0 ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosCompareInfoIsSortableInt(CHAOS_IL2CPP_INT32 ch) noexcept
{
    // All valid Unicode scalar values are sortable; suppress unused warning.
    (void)ch;
    return 1;
}

CHAOS_IL2CPP_INTPTR ChaosDateTimeFormatInfoGetInstance(CHAOS_IL2CPP_INTPTR /*provider*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfo(CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoBool(CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 /*tryFirst*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(name ? s_stub_culture : 0);
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoByIetfLanguageTag(CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCultureCreateSpecificCulture(CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_culture);
}

CHAOS_IL2CPP_INTPTR ChaosCompareInfoGetCompareInfo(CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_stub_subsystem);
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
    Region* young_region = G_YoungGen().region.load(std::memory_order_acquire);
    if (young_region != nullptr && young_region->current > young_region->begin) {
        uint32_t gen = chaos::il2cpp::runtime_core::threading::RequestGlobalSafepoint();
        GcYoungCollection();
        chaos::il2cpp::runtime_core::threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 2: Full collection when gen >= 1 or default (-1).
    if (generation < 0 || generation >= 1) {
        uint32_t gen = chaos::il2cpp::runtime_core::threading::RequestGlobalSafepoint();
        G_OldGen().Collect(nullptr, nullptr);
        chaos::il2cpp::runtime_core::threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 3: Run pending finalizers.
    G_OldGen().RunFinalizers();
}

extern "C" {
CHAOS_IL2CPP_INT32 ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    if (obj == 0) return 0;
    void* ptr = reinterpret_cast<void*>(obj);
    if (RegionManager::Instance().IsNurseryPointer(ptr)) return 0;
    if (G_OldGen().IsInOldGen(ptr)) return 2;
    if (G_Loh().IsInLOH(ptr)) return 2;
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
    chaos::il2cpp::runtime_core::G_OldGen().SuppressFinalizer(reinterpret_cast<void*>(obj));
}

void chaos_gc_reregister_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept {
    if (obj == 0) return;
    chaos::il2cpp::runtime_core::G_OldGen().ReRegisterFinalizer(reinterpret_cast<void*>(obj));
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core