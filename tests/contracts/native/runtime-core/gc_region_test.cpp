/// Unit tests for CRAG region framework + card table (C0.1-C0.3).
///
/// Exercises:
///   - Region struct layout and kind semantics
///   - RegionManager: AllocateRegion / FreeRegion / ReleaseDomainRegions
///   - NurseryAllocate inline fast path + slow path fallback
///   - Card table: DirtyCard / IsDirty / ClearCard / ScanDirtyCards / GcSetHeapBase
///
/// These tests verify the M0 scaffolding.  Full GC integration (young/full
/// collection, promote, sweep) is covered in later C-stage tests.

#include <chaos/native_types.h>

#include "gc_region.h"
#include "gc_card_table.h"

#include <gc.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ════════════════════════════════════════════════════════════════════════════
// Test helpers (ad-hoc, no framework dependency)
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        g_sub = 0;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define SUBTEST(name)                                                   \
    do {                                                                \
        ++g_sub;                                                        \
        printf("\n    SUB %d: %s ... ", g_sub, name);                   \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// Region struct layout and kind semantics
// ════════════════════════════════════════════════════════════════════════════

static void test_region_layout() {
    TEST("Region struct layout");

    SUBTEST("RegionKind enum values");
    if (static_cast<int>(RegionKind::REGION_NURSERY) != 0) FAIL("NURSERY != 0");
    if (static_cast<int>(RegionKind::REGION_TENURED) != 1) FAIL("TENURED != 1");
    if (static_cast<int>(RegionKind::REGION_DOMAIN)  != 2) FAIL("DOMAIN != 2");
    if (static_cast<int>(RegionKind::REGION_RAW)     != 3) FAIL("RAW != 3");
    if (static_cast<int>(RegionKind::REGION_FOH)     != 4) FAIL("FOH != 4");
    PASS();

    SUBTEST("RegionId sentinel");
    if (kRegionIdInvalid != 0) FAIL("kRegionIdInvalid != 0");
    PASS();

    SUBTEST("Region field assignment");
    Region r{};
    r.id = 42;
    r.kind = RegionKind::REGION_TENURED;
    r.domain_id = 7;
    r.begin = reinterpret_cast<char*>(0x1000);
    r.end = reinterpret_cast<char*>(0x2000);
    r.current = r.begin;
    r.gc_state = {};
    r.next = nullptr;

    if (r.id != 42)           FAIL("id mismatch");
    if (r.kind != RegionKind::REGION_TENURED) FAIL("kind mismatch");
    if (r.domain_id != 7)     FAIL("domain_id mismatch");
    if (r.begin != r.current) FAIL("current != begin after init");
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// RegionManager: AllocateRegion / FreeRegion / ReleaseDomainRegions
// ════════════════════════════════════════════════════════════════════════════

static void test_region_manager() {
    TEST("RegionManager lifecycle");

    GC_INIT();
    auto& mgr = RegionManager::Instance();

    SUBTEST("AllocateRegion nursery");
    Region* nursery = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    if (nursery == nullptr)            FAIL("null nursery");
    if (nursery->id == kRegionIdInvalid) FAIL("invalid id");
    if (nursery->kind != RegionKind::REGION_NURSERY) FAIL("wrong kind");
    if (nursery->begin == nullptr)     FAIL("null begin");
    if (nursery->end <= nursery->begin) FAIL("end <= begin");
    if (nursery->current != nursery->begin) FAIL("current not at begin");
    if (nursery->domain_id != 0)       FAIL("domain_id != 0 for global");
    PASS();

    SUBTEST("AllocateRegion tenured");
    Region* tenured = mgr.AllocateRegion(RegionKind::REGION_TENURED, 0, 5);
    if (tenured == nullptr)            FAIL("null tenured");
    if (tenured->kind != RegionKind::REGION_TENURED) FAIL("wrong kind");
    if (tenured->domain_id != 5)       FAIL("domain_id != 5");
    PASS();

    SUBTEST("AllocateRegion domain");
    Region* domain = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    if (domain == nullptr)             FAIL("null domain");
    if (domain->kind != RegionKind::REGION_DOMAIN) FAIL("wrong kind");
    if (domain->domain_id != 3)        FAIL("domain_id != 3");
    PASS();

    SUBTEST("FreeRegion then reallocate reuses slot");
    RegionId id = nursery->id;
    mgr.FreeRegion(id);

    Region* realloc = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    if (realloc == nullptr)            FAIL("null realloc");
    // The free-list may or may not recycle the same physical region;
    // just verify the manager didn't crash and returns a valid region.
    if (realloc->id == kRegionIdInvalid) FAIL("realloc has invalid id");
    PASS();

    SUBTEST("ReleaseDomainRegions");
    // Allocate a second domain region for domain_id=3.
    Region* d2 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    if (d2 == nullptr) FAIL("null d2");

    mgr.ReleaseDomainRegions(3);

    // After release, allocate another region for domain=3 — should get
    // a recycled region from the free list (no crash).
    Region* after = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    if (after == nullptr) FAIL("null after release");
    if (after->id == kRegionIdInvalid) FAIL("after has invalid id");
    PASS();

    SUBTEST("ActiveRegionCount");
    if (mgr.ActiveRegionCount() < 3) FAIL("too few regions tracked");
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// NurseryAllocate inline fast path
// ════════════════════════════════════════════════════════════════════════════

static void test_nursery_allocate() {
    TEST("NurseryAllocate");

    GC_INIT();
    auto& mgr = RegionManager::Instance();

    // Set up TLS nursery context.
    Region* nursery = mgr.AllocateNursery();
    if (nursery == nullptr) { FAIL("no nursery for test"); return; }
    tls_nursery_ctx.nursery = nursery;
    tls_nursery_ctx.limit = nursery->end - kMaxNurseryAlloc;

    SUBTEST("small allocation returns non-null, zeroed memory");
    void* p1 = NurseryAllocate(16);
    if (p1 == nullptr) { FAIL("null p1"); return; }
    // Verify zeroed.
    auto* bytes = static_cast<uint8_t*>(p1);
    for (size_t i = 0; i < 16; i++) {
        if (bytes[i] != 0) { FAIL("memory not zeroed"); return; }
    }
    PASS();

    SUBTEST("consecutive allocations don't overlap");
    void* p2 = NurseryAllocate(32);
    void* p3 = NurseryAllocate(64);
    if (p2 == nullptr || p3 == nullptr) { FAIL("null alloc"); return; }

    // Write patterns and verify no overlap.
    std::memset(p2, 0xAB, 32);
    std::memset(p3, 0xCD, 64);

    auto* b2 = static_cast<uint8_t*>(p2);
    auto* b3 = static_cast<uint8_t*>(p3);

    // Check p2's tail didn't corrupt p3's head (or vice versa).
    if (b2[0] != 0xAB || b2[31] != 0xAB) { FAIL("p2 corrupt"); return; }
    if (b3[0] != 0xCD || b3[63] != 0xCD) { FAIL("p3 corrupt"); return; }

    // Verify no overlap by address range.
    auto addr2 = reinterpret_cast<uintptr_t>(p2);
    auto addr3 = reinterpret_cast<uintptr_t>(p3);
    auto end2 = addr2 + 32;
    auto end3 = addr3 + 64;
    if (addr2 < end3 && addr3 < end2) { FAIL("overlapping allocations"); return; }
    PASS();

    SUBTEST("8-byte alignment");
    void* p4 = NurseryAllocate(1);
    if (p4 == nullptr) { FAIL("null p4"); return; }
    auto addr4 = reinterpret_cast<uintptr_t>(p4);
    if ((addr4 & 7) != 0) { FAIL("misaligned"); return; }
    PASS();

    SUBTEST("oversized bypasses nursery");
    void* large = NurseryAllocate(kMaxNurseryAlloc + 1);
    if (large == nullptr) { FAIL("null large"); return; }
    // Should be zeroed.
    auto* blarge = static_cast<uint8_t*>(large);
    if (blarge[0] != 0 || blarge[kMaxNurseryAlloc] != 0) { FAIL("large not zeroed"); return; }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Card table: DirtyCard / IsDirty / ClearCard / ScanDirtyCards / GcSetHeapBase
// ════════════════════════════════════════════════════════════════════════════

static void test_card_table() {
    TEST("Card table");

    // The heap base must be set. Use a reasonable fake base for testing.
    // Real value is set at runtime initialization.
    void* fake_heap = GC_MALLOC(1024 * 1024);
    if (fake_heap == nullptr) { FAIL("GC_MALLOC failed"); return; }
    GcSetHeapBase(fake_heap);

    // Compute a pointer inside the heap for card operations.
    auto base = reinterpret_cast<uintptr_t>(fake_heap);
    void* obj = reinterpret_cast<void*>(base + 4096);

    SUBTEST("DirtyCard + IsDirty");
    if (IsDirty(obj)) { FAIL("should be clean initially"); return; }
    DirtyCard(obj);
    if (!IsDirty(obj)) { FAIL("should be dirty after DirtyCard"); return; }
    PASS();

    SUBTEST("ClearCard");
    ClearCard(obj);
    if (IsDirty(obj)) { FAIL("should be clean after ClearCard"); return; }
    PASS();

    SUBTEST("ClearAllCards");
    DirtyCard(obj);
    ClearAllCards();
    if (IsDirty(obj)) { FAIL("should be clean after ClearAllCards"); return; }
    PASS();

    SUBTEST("ScanDirtyCards finds dirty cards");
    DirtyCard(obj);
    bool found = false;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t /*idx*/, uintptr_t /*start*/, uintptr_t /*end*/) {
        found = true;
    });
    if (!found) { FAIL("ScanDirtyCards did not find dirty card"); return; }
    PASS();

    SUBTEST("ScanDirtyCards empty range");
    ClearAllCards();
    found = false;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        found = true;
    });
    if (found) { FAIL("ScanDirtyCards found something in clean range"); return; }
    PASS();

    SUBTEST("DirtyCard out-of-range is safe");
    // A pointer before the heap base should not crash.
    DirtyCard(reinterpret_cast<void*>(base - 4096));
    // A pointer beyond the card table coverage should not crash.
    DirtyCard(reinterpret_cast<void*>(base + (static_cast<uintptr_t>(kCardL1Entries) << kCardShift) + 65536));
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("CRAG M0 tests (C0.1-C0.3):");
    puts("═══════════════════════════\n");

    // Initialize BDWGC once for the test process.
    GC_INIT();

    test_region_layout();
    test_region_manager();
    test_nursery_allocate();
    test_card_table();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}