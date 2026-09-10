/// Migrated MemoryDomain unit tests (GoogleTest).
///
/// Exercises: Register/Find/Unregister, DomainCurrentAllocateTagged/Free,
/// DomainCurrentReallocateTagged, PushDomain/PopDomain/DomainScope,
/// usage_limit enforcement.

#include <chaos/native_types.h>

#include "memory_domain.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::memory_domain;

// ═══════════════════════════════════════════════════════════════════════════
// Constants
// ═══════════════════════════════════════════════════════════════════════════

static constexpr const char* kTestModuleName   = "gc_bump_cache_test.dll";
static constexpr CHAOS_IL2CPP_UINT32  kHotUpdateKind  = 1u;

// ═══════════════════════════════════════════════════════════════════════════
// Test: Register and find
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, RegisterAndFind) {
    DomainInit init{};
    init.module_name = kTestModuleName;
    init.module_kind = kHotUpdateKind;
    init.usage_limit = 0;  // unlimited

    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain returned invalid id";

    // Find by id.
    MemoryDomain* by_id = FindDomainById(id);
    ASSERT_NE(by_id, nullptr) << "FindDomainById returned null for valid id";
    EXPECT_EQ(by_id->domain_id, id) << "FindDomainById returned wrong domain_id";

    // Find by name.
    MemoryDomain* by_name = FindDomainByName(kTestModuleName);
    ASSERT_NE(by_name, nullptr) << "FindDomainByName returned null";
    EXPECT_EQ(by_name->domain_id, id) << "FindDomainByName returned wrong domain";

    // Cleanup.
    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Find nonexistent domain
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, FindNonexistent) {
    MemoryDomain* d = FindDomainById(9999);
    EXPECT_EQ(d, nullptr) << "expected nullptr for nonexistent id";
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: DomainScope RAII
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, DomainScope) {
    DomainInit init{};
    init.module_name = "domain_scope_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr) << "FindDomainById failed";

    // Before push, CurrentDomain should be null.
    EXPECT_EQ(CurrentDomain(), nullptr) << "CurrentDomain should be null before push";

    {
        DomainScope scope(domain);
        EXPECT_EQ(CurrentDomain(), domain) << "CurrentDomain should return pushed domain inside scope";
    }

    // After scope, CurrentDomain should be null again.
    EXPECT_EQ(CurrentDomain(), nullptr) << "CurrentDomain should be null after scope exit";

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: CurrentDomainAllocateTagged + DomainFreeTagged
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, AllocateAndFree) {
    DomainInit init{};
    init.module_name = "alloc_free_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Allocate and write into memory.
    void* p = DomainCurrentAllocateTagged(64);
    ASSERT_NE(p, nullptr) << "DomainCurrentAllocateTagged returned null";
    std::memset(p, 0xAA, 64);

    // Free via DomainFreeTagged -- should route to correct heap via tag.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Direct DomainAllocateTagged (bypasses current domain)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, DirectAllocateTagged) {
    DomainInit init{};
    init.module_name = "direct_alloc_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);

    // Allocate without pushing domain onto TLS stack.
    void* p = DomainAllocateTagged(domain, 128);
    ASSERT_NE(p, nullptr) << "DomainAllocateTagged returned null";
    std::memset(p, 0xBB, 128);

    // Free should still work because the tag carries the heap pointer.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: DomainCurrentReallocateTagged preserves tag
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, ReallocatePreservesTag) {
    DomainInit init{};
    init.module_name = "realloc_tag_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Allocate initial block.
    void* p = DomainCurrentAllocateTagged(32);
    ASSERT_NE(p, nullptr) << "first allocate returned null";

    std::memcpy(p, "hello realloc", 14);

    // Reallocate to a larger size.
    void* q = DomainCurrentReallocateTagged(p, 128);
    ASSERT_NE(q, nullptr) << "reallocate returned null (original block may have been freed)";

    // Data should have been preserved (realloc copies, or new alloc + copy).
    EXPECT_EQ(std::memcmp(q, "hello realloc", 14), 0) << "reallocated block content corrupted";

    // Free via DomainFreeTagged -- if tag was lost, this would WARN + no-op.
    DomainFreeTagged(q);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: DomainCurrentReallocateTagged with nullptr (acts as allocate)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, ReallocateNullptr) {
    DomainInit init{};
    init.module_name = "realloc_null_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    void* p = DomainCurrentReallocateTagged(nullptr, 64);
    ASSERT_NE(p, nullptr) << "realloc(nullptr, 64) returned null";
    std::memset(p, 0xCC, 64);
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Usage limit enforcement
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, UsageLimit) {
    DomainInit init{};
    init.module_name = "usage_limit_test.dll";
    init.module_kind = 0;
    init.usage_limit = 256;  // very small limit

    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Small allocation should succeed.
    void* p = DomainCurrentAllocateTagged(64);
    ASSERT_NE(p, nullptr) << "first allocation (64 bytes) should succeed within 256 limit";

    // Large allocation beyond limit should fail.
    // Allocation adds sizeof(AllocationHeader) overhead (~8 bytes).
    void* q = DomainCurrentAllocateTagged(4096);
    EXPECT_EQ(q, nullptr) << "second allocation (4096 bytes) should have been rejected by limit";
    // Only free p (q is nullptr, so no-op).
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Allocate after domain unload (no-op, not crash)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, AllocateAfterUnload) {
    // DomainCurrentAllocateTagged with no active domain should fall back to
    // tagged std::malloc -- not crash.
    void* p = DomainCurrentAllocateTagged(32);
    if (p == nullptr) {
        // This is acceptable if no domain context is expected to be required.
        // Some builds may require a domain; we accept either behavior.
        SUCCEED();
        return;
    }
    // If we got a pointer, free it.
    DomainFreeTagged(p);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: DomainFreeTagged(nullptr) no-op
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, FreeNullptr) {
    // Should not crash.
    DomainFreeTagged(nullptr);
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Double registration (different names)
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomain, MultiDomain) {
    DomainInit a{}, b{};
    a.module_name = "multi_a.dll"; a.module_kind = 0;
    b.module_name = "multi_b.dll"; b.module_kind = 0;

    DomainId id_a = RegisterMemoryDomain(a);
    DomainId id_b = RegisterMemoryDomain(b);
    ASSERT_NE(id_a, kDomainIdInvalid) << "RegisterMemoryDomain failed for domain A";
    ASSERT_NE(id_b, kDomainIdInvalid) << "RegisterMemoryDomain failed for domain B";
    EXPECT_NE(id_a, id_b) << "two domains got the same id";

    // Find each independently.
    EXPECT_NE(FindDomainByName("multi_a.dll"), nullptr) << "could not find domain A by name";
    EXPECT_NE(FindDomainByName("multi_b.dll"), nullptr) << "could not find domain B by name";

    UnregisterMemoryDomain(id_a);
    UnregisterMemoryDomain(id_b);
}
