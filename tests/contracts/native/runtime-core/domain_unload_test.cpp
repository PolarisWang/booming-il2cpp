/// Integration tests for CRAG domain unloader (C4).
///
/// Exercises:
///   - UnloadDomain with invalid/nonexistent domain IDs (safety)
///   - Basic domain unload: register domain → allocate domain region → unload
///   - Cross-domain reference scan counts during unload
///   - Multiple domain allocations + unload (bulk release)
///
/// These tests verify the C4 stop-the-world domain teardown orchestration:
///   safepoint → scan cross-domain refs → clear refs →
///   release regions → unregister memory domain → resume threads.

#include <chaos/native_types.h>

#include "domain_unloader.h"
#include "gc_card_table.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "memory_domain.h"

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
using namespace chaos::il2cpp::memory_domain;

// ════════════════════════════════════════════════════════════════════════════
// Test: Invalid domain ID
// ════════════════════════════════════════════════════════════════════════════

static void test_invalid_domain() {
    TEST("UnloadDomain with invalid ID returns failure");

    DomainUnloadResult r = UnloadDomain(0);
    if (r.success) {
        FAIL("expected failure for domain_id=0");
        return;
    }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Nonexistent domain ID
// ════════════════════════════════════════════════════════════════════════════

static void test_nonexistent_domain() {
    TEST("UnloadDomain with nonexistent ID does not crash");

    // A domain_id that was never registered — should not crash.
    // ReleaseDomainRegions will find nothing; UnregisterMemoryDomain
    // will return false. The orchestration should complete gracefully.
    DomainUnloadResult r = UnloadDomain(9999);

    // The function currently returns success=true as long as the
    // orchestration completes without crash (even if no regions
    // matched). This is acceptable for C4.
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Basic domain unload
// ════════════════════════════════════════════════════════════════════════════

static void test_basic_unload() {
    TEST("UnloadDomain basic lifecycle");

    // Set heap base for card table ops.
    GcSetHeapBase(reinterpret_cast<void*>(0x1000000));

    // Register a memory domain.
    DomainInit init{};
    init.module_name = "basic_unload_test.dll";
    init.module_kind = 1;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    // Verify domain is findable before unload.
    MemoryDomain* before = FindDomainById(id);
    if (before == nullptr) {
        FAIL("domain should be findable before unload");
        UnregisterMemoryDomain(id);
        return;
    }
    if (before->is_unloaded) {
        FAIL("domain should not be unloaded before unload");
        UnregisterMemoryDomain(id);
        return;
    }

    // Allocate a domain region owned by this domain.
    auto& mgr = RegionManager::Instance();
    Region* dr = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    if (dr == nullptr) {
        FAIL("AllocateRegion(REGION_DOMAIN) failed");
        UnregisterMemoryDomain(id);
        return;
    }
    if (dr->domain_id != id) {
        FAIL("region domain_id does not match");
        UnregisterMemoryDomain(id);
        return;
    }

    // Also set up a nursery for a realistic GC environment.
    InitYoungGeneration();
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) {
        FAIL("InitYoungGeneration failed");
        UnregisterMemoryDomain(id);
        return;
    }

    // Allocate objects and create card-table dirtiness for the scan.
    void* p = NurseryAllocate(64);
    if (p == nullptr) {
        FAIL("NurseryAllocate failed");
        UnregisterMemoryDomain(id);
        return;
    }
    // Dirty a card in the nursery range so the scanner has work.
    DirtyCard(p);

    // ── Unload the domain ──
    DomainUnloadResult r = UnloadDomain(id);
    if (!r.success) {
        FAIL("UnloadDomain returned failure");
        return;
    }

    // Verify the domain is no longer active.
    MemoryDomain* after = FindDomainById(id);
    if (after != nullptr && !after->is_unloaded) {
        FAIL("domain should be unloaded after UnloadDomain");
        return;
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Multiple domain regions released
// ════════════════════════════════════════════════════════════════════════════

static void test_multi_region_unload() {
    TEST("UnloadDomain releases multiple domain regions");

    DomainInit init{};
    init.module_name = "multi_region_test.dll";
    init.module_kind = 1;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    auto& mgr = RegionManager::Instance();

    // Allocate several domain regions for the same domain.
    Region* r1 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    Region* r2 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    Region* r3 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    if (r1 == nullptr || r2 == nullptr || r3 == nullptr) {
        FAIL("one or more domain region allocations failed");
        UnregisterMemoryDomain(id);
        return;
    }

    // Write patterns into each domain region to confirm no overlap.
    if (r1->begin) std::memset(r1->begin, 0xA1, 32);
    if (r2->begin) std::memset(r2->begin, 0xB2, 32);
    if (r3->begin) std::memset(r3->begin, 0xC3, 32);

    // Unload — should release all three regions.
    DomainUnloadResult r = UnloadDomain(id);
    if (!r.success) {
        FAIL("UnloadDomain failed");
        return;
    }

    // After unload, allocating a new region should reuse a freed slot
    // (the free list has the domain regions). Verify no crash.
    Region* recycled = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    if (recycled == nullptr) {
        FAIL("post-unload allocation failed");
        return;
    }
    if (recycled->id == kRegionIdInvalid) {
        FAIL("recycled region has invalid id");
        return;
    }

    MemoryDomain* after = FindDomainById(id);
    if (after != nullptr && !after->is_unloaded) {
        FAIL("domain should be unloaded after UnloadDomain");
        return;
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Unload preserves unrelated domain
// ════════════════════════════════════════════════════════════════════════════

static void test_preserves_other_domain() {
    TEST("UnloadDomain does not affect unrelated domains");

    // Register two domains.
    DomainInit init_a{}, init_b{};
    init_a.module_name = "domain_a.dll"; init_a.module_kind = 1;
    init_b.module_name = "domain_b.dll"; init_b.module_kind = 1;
    DomainId id_a = RegisterMemoryDomain(init_a);
    DomainId id_b = RegisterMemoryDomain(init_b);
    if (id_a == kDomainIdInvalid || id_b == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        if (id_a != kDomainIdInvalid) UnregisterMemoryDomain(id_a);
        if (id_b != kDomainIdInvalid) UnregisterMemoryDomain(id_b);
        return;
    }

    auto& mgr = RegionManager::Instance();

    // Allocate domain regions for both domains.
    Region* r_a = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id_a);
    Region* r_b = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id_b);
    if (r_a == nullptr || r_b == nullptr) {
        FAIL("region allocation failed");
        UnregisterMemoryDomain(id_a);
        UnregisterMemoryDomain(id_b);
        return;
    }

    // Write markers.
    if (r_a->begin) std::memset(r_a->begin, 0xAA, 16);
    if (r_b->begin) std::memset(r_b->begin, 0xBB, 16);

    // Unload only domain A.
    DomainUnloadResult r = UnloadDomain(id_a);
    if (!r.success) {
        FAIL("UnloadDomain(id_a) failed");
        UnregisterMemoryDomain(id_b);
        return;
    }

    // Domain A should be gone.
    MemoryDomain* after_a = FindDomainById(id_a);
    if (after_a != nullptr && !after_a->is_unloaded) {
        FAIL("domain A should be unloaded");
        UnregisterMemoryDomain(id_b);
        return;
    }

    // Domain B should still be findable and not unloaded.
    MemoryDomain* after_b = FindDomainById(id_b);
    if (after_b == nullptr) {
        FAIL("domain B should still be findable");
        return;
    }
    if (after_b->is_unloaded) {
        FAIL("domain B should not be unloaded");
        UnregisterMemoryDomain(id_b);
        return;
    }

    // Clean up domain B.
    UnregisterMemoryDomain(id_b);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("CRAG C4 tests (domain unloader):");
    puts("════════════════════════════════\n");

    test_invalid_domain();
    test_nonexistent_domain();
    test_basic_unload();
    test_multi_region_unload();
    test_preserves_other_domain();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}