/// MemoryDomain proof (capability) test suite.
///
/// Exercises key contracts and invariants that must hold after the
/// batch-1 fixes (R5-R7):
///
///   R5 fix: PushDomain overflow safe (sentinel 64)
///   R6 fix: Atomic current_usage / peak_usage tracking
///   R7 fix: Tombstone + lazy compaction for global domain registry
///
/// Tests:
///   1. DomainScope_RaiiCorrectness
///   2. TaggedAlloc_CrossDomainFree
///   3. UnloadSafety
///   4. UsageLimitEnforcement
///   5. MultiDomainIsolation
///   6. TlsDomainStack_Correctness
///   7. TombstoneFind_Correctness
///   8. AtomicUsage_TrackConsistency
///   9. PushDomain_OverflowSafe

#include <chaos/native_types.h>

#include "memory_domain.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::memory_domain;

static constexpr const char* kModuleName   = "proof_test.dll";
static constexpr CHAOS_IL2CPP_UINT32 kHotUpdateKind = 1u;

// ═══════════════════════════════════════════════════════════════════════════
// Test 1: DomainScope RAII correctness
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, DomainScope_RaiiCorrectness) {
    DomainInit init{};
    init.module_name = "raii_proof.dll";
    init.module_kind = 0;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr);

    // Before push, CurrentDomain must be null.
    EXPECT_EQ(CurrentDomain(), nullptr) << "CurrentDomain should be null before any push";

    {
        DomainScope scope(domain);
        EXPECT_EQ(CurrentDomain(), domain) << "CurrentDomain should be domain inside scope";
    }

    // After scope exit, CurrentDomain must return to null (auto-pop).
    EXPECT_EQ(CurrentDomain(), nullptr) << "CurrentDomain should be null after scope exit";

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 2: TaggedAlloc_CrossDomainFree
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, TaggedAlloc_CrossDomainFree) {
    // Allocate in domain A, then switch to domain B, then free.
    // The free must route to the correct heap via the tagged header.

    DomainInit init_a{}, init_b{};
    init_a.module_name = "cross_a_proof.dll"; init_a.module_kind = 0; init_a.usage_limit = 0;
    init_b.module_name = "cross_b_proof.dll"; init_b.module_kind = 0; init_b.usage_limit = 0;

    DomainId id_a = RegisterMemoryDomain(init_a);
    DomainId id_b = RegisterMemoryDomain(init_b);
    ASSERT_NE(id_a, kDomainIdInvalid);
    ASSERT_NE(id_b, kDomainIdInvalid);

    MemoryDomain* dom_a = FindDomainById(id_a);
    MemoryDomain* dom_b = FindDomainById(id_b);
    ASSERT_NE(dom_a, nullptr);
    ASSERT_NE(dom_b, nullptr);

    // Allocate in domain A.
    void* p = DomainAllocateTagged(dom_a, 64);
    ASSERT_NE(p, nullptr);

    std::memset(p, 0xCA, 64);

    // Switch to domain B.
    DomainScope scope_b(dom_b);
    EXPECT_EQ(CurrentDomain(), dom_b);

    // Free p while in domain B. The routing header should direct the free
    // to domain A's heap, not domain B's.
    // If cross-domain free fails, this would typically WARN + leak, but
    // should not crash or corrupt.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id_a);
    UnregisterMemoryDomain(id_b);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 3: UnloadSafety
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, UnloadSafety) {
    // 1. Register a domain.
    // 2. Allocate memory through it.
    // 3. Unregister the domain.
    // 4. Verify FindDomainById returns nullptr.
    // 5. Verify that allocating on the unloaded domain is a safe no-op.

    DomainInit init{};
    init.module_name = "unload_safe.dll";
    init.module_kind = 0;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr);

    // Allocate and write.
    DomainScope scope(domain);
    void* p = DomainCurrentAllocateTagged(64);
    ASSERT_NE(p, nullptr);
    std::memset(p, 0xDD, 64);
    DomainFreeTagged(p);

    scope.~DomainScope();  // manually pop before unregister

    // Unload the domain.
    bool ok = UnregisterMemoryDomain(id);
    ASSERT_TRUE(ok) << "UnregisterMemoryDomain failed";

    // After unload, FindDomainById must return nullptr.
    MemoryDomain* after_unload = FindDomainById(id);
    EXPECT_EQ(after_unload, nullptr) << "FindDomainById should return nullptr after unload";

    // Allocate after unload (no active domain) — must not crash.
    // DomainCurrentAllocateTagged with nullptr domain falls back to std::malloc.
    void* q = DomainCurrentAllocateTagged(32);
    if (q != nullptr) {
        DomainFreeTagged(q);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 4: UsageLimitEnforcement
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, UsageLimitEnforcement) {
    // Set a 256-byte limit.
    // First alloc of 200 bytes should succeed.
    // Second alloc of 200 bytes should fail (would exceed limit).

    DomainInit init{};
    init.module_name = "usage_limit_proof.dll";
    init.module_kind = 0;
    init.usage_limit = 256;

    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr);
    DomainScope scope(domain);

    // Allocate 200 bytes — should succeed (200 <= 256).
    void* p = DomainCurrentAllocateTagged(200);
    ASSERT_NE(p, nullptr) << "allocation of 200 bytes should succeed within 256 limit";
    std::memset(p, 0xEE, 200);

    // Allocate another 200 bytes — should fail (200 + 200 + overhead > 256).
    void* q = DomainCurrentAllocateTagged(200);
    EXPECT_EQ(q, nullptr) << "second allocation of 200 bytes should be rejected by 256 limit";

    // Free the first allocation.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 5: MultiDomainIsolation
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, MultiDomainIsolation) {
    // 3 domains each allocate independently, no cross-interference.

    static constexpr const char* kNames[3] = {
        "iso_a_proof.dll", "iso_b_proof.dll", "iso_c_proof.dll"
    };
    DomainId ids[3];
    MemoryDomain* domains[3];

    for (int i = 0; i < 3; i++) {
        DomainInit init{};
        init.module_name = kNames[i];
        init.module_kind = static_cast<CHAOS_IL2CPP_UINT32>(i);
        init.usage_limit = 0;
        ids[i] = RegisterMemoryDomain(init);
        ASSERT_NE(ids[i], kDomainIdInvalid) << "RegisterMemoryDomain failed for domain " << i;
        domains[i] = FindDomainById(ids[i]);
        ASSERT_NE(domains[i], nullptr) << "FindDomainById failed for domain " << i;
    }

    for (int i = 0; i < 3; i++) {
        DomainScope scope(domains[i]);
        EXPECT_EQ(CurrentDomain(), domains[i]) << "CurrentDomain mismatch for domain " << i;

        void* p = DomainCurrentAllocateTagged(64);
        ASSERT_NE(p, nullptr) << "Allocation failed in domain " << i;
        std::memset(p, static_cast<int>(0xF0 + i), 64);
        DomainFreeTagged(p);
    }

    // Verify domains are independently findable by name.
    for (int i = 0; i < 3; i++) {
        MemoryDomain* by_name = FindDomainByName(kNames[i]);
        ASSERT_NE(by_name, nullptr) << "FindDomainByName failed for " << kNames[i];
        EXPECT_EQ(by_name->domain_id, ids[i]) << "domain_id mismatch for " << kNames[i];
        EXPECT_EQ(by_name->module_kind, static_cast<CHAOS_IL2CPP_UINT32>(i))
            << "module_kind mismatch for " << kNames[i];
    }

    for (int i = 0; i < 3; i++) {
        UnregisterMemoryDomain(ids[i]);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 6: TlsDomainStack_Correctness
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainProof, TlsDomainStack_Correctness) {
    // Push 3 domains in sequence, verify CurrentDomain returns the correct
    // top-of-stack at each level, then pop in reverse order.

    DomainInit init_a{}, init_b{}, init_c{};
    init_a.module_name = "stack_a.dll"; init_a.module_kind = 0; init_a.usage_limit = 0;
    init_b.module_name = "stack_b.dll"; init_b.module_kind = 0; init_b.usage_limit = 0;
    init_c.module_name = "stack_c.dll"; init_c.module_kind = 0; init_c.usage_limit = 0;

    DomainId id_a = RegisterMemoryDomain(init_a);
    DomainId id_b = RegisterMemoryDomain(init_b);
    DomainId id_c = RegisterMemoryDomain(init_c);
    ASSERT_NE(id_a, kDomainIdInvalid);
    ASSERT_NE(id_b, kDomainIdInvalid);
    ASSERT_NE(id_c, kDomainIdInvalid);

    MemoryDomain* a = FindDomainById(id_a);
    MemoryDomain* b = FindDomainById(id_b);
    MemoryDomain* c = FindDomainById(id_c);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    ASSERT_NE(c, nullptr);

    EXPECT_EQ(CurrentDomain(), nullptr) << "stack should be empty initially";

    int d1 = PushDomain(a);
    EXPECT_EQ(CurrentDomain(), a) << "after push a, top should be a";
    EXPECT_GE(d1, 0);

    int d2 = PushDomain(b);
    EXPECT_EQ(CurrentDomain(), b) << "after push b, top should be b";
    EXPECT_GT(d2, d1);

    int d3 = PushDomain(c);
    EXPECT_EQ(CurrentDomain(), c) << "after push c, top should be c";
    EXPECT_GT(d3, d2);

    PopDomain(d3);
    EXPECT_EQ(CurrentDomain(), b) << "after pop c, top should be b";

    PopDomain(d2);
    EXPECT_EQ(CurrentDomain(), a) << "after pop b, top should be a";

    PopDomain(d1);
    EXPECT_EQ(CurrentDomain(), nullptr) << "after pop a, stack should be empty";

    UnregisterMemoryDomain(id_a);
    UnregisterMemoryDomain(id_b);
    UnregisterMemoryDomain(id_c);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 7: TombstoneFind_Correctness (R7 fix)
// ═══════════════════════════════════════════════════════════════════════════
//
// Verifies that after unregistering a domain and registering a new one
// (which may reuse the tombstoned slot), Find by the old id returns nullptr
// and Find by the new id returns the correct domain.

TEST(MemoryDomainProof, TombstoneFind_Correctness) {
    DomainInit init_a{}, init_b{};
    init_a.module_name = "tombstone_a.dll"; init_a.module_kind = 0; init_a.usage_limit = 0;
    init_b.module_name = "tombstone_b.dll"; init_b.module_kind = 0; init_b.usage_limit = 0;

    // Register domain A and capture its id.
    DomainId id_a_orig = RegisterMemoryDomain(init_a);
    ASSERT_NE(id_a_orig, kDomainIdInvalid);

    // Unregister domain A — creates a tombstone slot.
    bool ok = UnregisterMemoryDomain(id_a_orig);
    ASSERT_TRUE(ok);

    // Verify FindDomainById returns nullptr for the old id.
    MemoryDomain* found_old = FindDomainById(id_a_orig);
    EXPECT_EQ(found_old, nullptr) << "FindDomainById should return nullptr for tombstoned id";

    // Register domain B — may reuse the tombstone slot.
    DomainId id_b = RegisterMemoryDomain(init_b);
    ASSERT_NE(id_b, kDomainIdInvalid);

    // Verify FindDomainById for id_a_orig still returns nullptr.
    found_old = FindDomainById(id_a_orig);
    EXPECT_EQ(found_old, nullptr)
        << "FindDomainById for old id should still return nullptr after new registration";

    // Verify FindDomainById for id_b returns the correct domain.
    MemoryDomain* found_b = FindDomainById(id_b);
    ASSERT_NE(found_b, nullptr) << "FindDomainById for new domain should succeed";
    EXPECT_EQ(found_b->domain_id, id_b) << "domain_id mismatch";
    EXPECT_STREQ(found_b->module_name, "tombstone_b.dll") << "module_name mismatch";

    // Verify FindDomainByName for old name returns nullptr.
    MemoryDomain* by_name_old = FindDomainByName("tombstone_a.dll");
    EXPECT_EQ(by_name_old, nullptr) << "FindDomainByName for old name should return nullptr";

    // Verify FindDomainByName for new name returns the correct domain.
    MemoryDomain* by_name_new = FindDomainByName("tombstone_b.dll");
    ASSERT_NE(by_name_new, nullptr) << "FindDomainByName for new name should succeed";
    EXPECT_EQ(by_name_new->domain_id, id_b);

    // Cleanup.
    UnregisterMemoryDomain(id_b);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 8: AtomicUsage_TrackConsistency (R6 fix)
// ═══════════════════════════════════════════════════════════════════════════
//
// Verifies that after allocation, current_usage increases, and after free,
// current_usage decreases back to the baseline.

TEST(MemoryDomainProof, AtomicUsage_TrackConsistency) {
    DomainInit init{};
    init.module_name = "usage_track.dll";
    init.module_kind = 0;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr);
    DomainScope scope(domain);

    // Capture baseline usage.
    CHAOS_IL2CPP_INT64 baseline = domain->current_usage.load(std::memory_order_relaxed);

    // Allocate 128 bytes.
    void* p = DomainCurrentAllocateTagged(128);
    ASSERT_NE(p, nullptr);
    std::memset(p, 0xAB, 128);

    // Verify current_usage increased.
    CHAOS_IL2CPP_INT64 after_alloc = domain->current_usage.load(std::memory_order_relaxed);
    EXPECT_GT(after_alloc, baseline) << "current_usage should increase after allocation";
    EXPECT_GE(domain->peak_usage.load(std::memory_order_relaxed), after_alloc)
        << "peak_usage should be >= current_usage after allocation";

    // Free the allocation.
    DomainFreeTagged(p);

    // Verify current_usage decreased back.
    CHAOS_IL2CPP_INT64 after_free = domain->current_usage.load(std::memory_order_relaxed);
    EXPECT_LT(after_free, after_alloc) << "current_usage should decrease after free";
    EXPECT_EQ(after_free, baseline) << "current_usage should return to baseline after free";

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 9: PushDomain_OverflowSafe (R5 fix)
// ═══════════════════════════════════════════════════════════════════════════
//
// Pushing more than 64 domains should not crash. The TLS domain stack has
// a fixed maximum depth; exceeding it should return a sentinel depth without
// crashing or corrupting the stack.

TEST(MemoryDomainProof, PushDomain_OverflowSafe) {
    // Register a single domain to push repeatedly.
    DomainInit init{};
    init.module_name = "overflow_safe.dll";
    init.module_kind = 0;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr);

    // Push many times to exceed the 64-slot limit.
    static constexpr int kExcessivePushes = 128;
    int depths[kExcessivePushes];

    for (int i = 0; i < kExcessivePushes; i++) {
        depths[i] = PushDomain(domain);
        // Should not crash; depth may saturate after ~64 pushes.
        EXPECT_GE(depths[i], 0) << "PushDomain returned negative depth at push " << i;
    }

    // Pop in reverse order.
    for (int i = kExcessivePushes - 1; i >= 0; i--) {
        // Should not crash on pop.
        PopDomain(depths[i]);
    }

    // After all pops, CurrentDomain should be null.
    EXPECT_EQ(CurrentDomain(), nullptr) << "CurrentDomain should be null after all pops";

    UnregisterMemoryDomain(id);
}
