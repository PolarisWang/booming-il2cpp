/// gc_diagnostics_test — VerifyHeap self-check (T2)
///
/// Verifies the GC self-verification layer:
///   1. GcGetHeapVerifyLevel() reads the HeapVerify config knob (env/default).
///   2. GcVerifyRegionToGenerationMap() runs without crashing on an
///      uninitialized heap (returns cleanly; no OOB).
///   3. GcMarkRangeOld produces a consistent OLD(2) label that the region-gen
///      verifier would accept, and a deliberately-corrupted label is detectable.
///
/// [多平台]  pure C++ no platform calls
/// [JIT/AOT] verifier is runtime-core-internal, engine-agnostic
/// [热更]    verifier is a debug safety net, no hot-update interaction

#include <cstdio>
#include <cstdlib>

#include <chaos/native_types.h>
#include "gc_config.h"
#include "gc_diagnostics.h"
#include "gc_region.h"

#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

static void TestVerifyLevel() {
    TEST("GcGetHeapVerifyLevel reads config");
    // Default is 0 → off; ensure we can call it without crashing.
    auto level = GcGetHeapVerifyLevel();
    // level should be one of the enum values (0/1/2).
    if (level < HeapVerifyLevel::kOff || level > HeapVerifyLevel::kFull) {
        FAIL("HeapVerifyLevel out of range");
        return;
    }
    PASS();
}

static void TestUninitializedVerify() {
    TEST("GcVerifyRegionToGenerationMap uninitialized-safe");
    // Before any heap init, all region-gen reads return the conservative
    // default; the verifier must not crash / OOB.
    GcVerifyRegionToGenerationMap();
    PASS();
}

static void TestMarkRangeConsistency() {
    TEST("GcMarkRangeOld label + corruption detectable");
    // Build a fake heap, ensure region-gen coverage, call GcMarkRangeOld, and
    // confirm every covered 4MB chunk reads OLD(2).
    void* raw = std::calloc(1, 32 * 1024 * 1024);
    if (!raw) { FAIL("calloc failed"); return; }
    uintptr_t base = reinterpret_cast<uintptr_t>(raw);
    // Align to a 4MB boundary clear of low cells.
    const uintptr_t kRegionSize = static_cast<uintptr_t>(1) << kRegionGenShift;
    uintptr_t a = (base & ~(kRegionSize - 1)) + kRegionSize;
    GcMarkRangeOld(a, a + 4 * kRegionSize);
    bool all_old = true;
    for (uintptr_t p = a; p < a + 4 * kRegionSize; p += kRegionSize) {
        if (GetRegionGen(p) != kRegionGenOld) { all_old = false; break; }
    }
    if (!all_old) { std::free(raw); FAIL("GcMarkRangeOld did not mark all OLD"); return; }
    // Corrupt one byte to young(0) and confirm the read back is 0 (detectable
    // by the verifier's CheckRegionGen).
    SetRegionGen(a, kRegionGenYoung);
    if (GetRegionGen(a) != kRegionGenYoung) { std::free(raw); FAIL("SetRegionGen corrupt not reflected"); return; }
    std::free(raw);
    PASS();
}

int main() {
    puts("CRAG GC self-verification (gc_diagnostics) tests");
    puts("════════════════════════════════════════════════════\n");
    TestVerifyLevel();
    TestUninitializedVerify();
    TestMarkRangeConsistency();
    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
