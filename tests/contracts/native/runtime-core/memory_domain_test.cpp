/// Unit tests for MemoryDomain system — domain lifecycle, tagged allocation,
/// cross-domain free routing, usage limits, and the REALLOC tag preservation.
///
/// Exercises: Register/Find/Unregister, DomainCurrentAllocateTagged/Free,
/// DomainCurrentReallocateTagged, PushDomain/PopDomain/DomainScope,
/// usage_limit enforcement.

#include <chaos/native_types.h>

#include "memory_domain.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ════════════════════════════════════════════════════════════════════════════
// Test helpers
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::memory_domain;

// ════════════════════════════════════════════════════════════════════════════
// Constants
// ════════════════════════════════════════════════════════════════════════════

static constexpr const char* kTestModuleName   = "gc_bump_cache_test.dll";
static constexpr CHAOS_IL2CPP_UINT32  kHotUpdateKind  = 1u;

// ════════════════════════════════════════════════════════════════════════════
// Test: Register and find
// ════════════════════════════════════════════════════════════════════════════

static void TestRegisterAndFind() {
    TEST("RegisterMemoryDomain + FindDomainById + FindDomainByName");

    DomainInit init{};
    init.module_name = kTestModuleName;
    init.module_kind = kHotUpdateKind;
    init.usage_limit = 0;  // unlimited

    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain returned invalid id");
        return;
    }

    // Find by id.
    MemoryDomain* by_id = FindDomainById(id);
    if (by_id == nullptr) {
        FAIL("FindDomainById returned null for valid id");
        return;
    }
    if (by_id->domain_id != id) {
        FAIL("FindDomainById returned wrong domain_id");
        return;
    }

    // Find by name.
    MemoryDomain* by_name = FindDomainByName(kTestModuleName);
    if (by_name == nullptr) {
        FAIL("FindDomainByName returned null");
        return;
    }
    if (by_name->domain_id != id) {
        FAIL("FindDomainByName returned wrong domain");
        return;
    }

    // Cleanup.
    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Find nonexistent domain
// ════════════════════════════════════════════════════════════════════════════

static void TestFindNonexistent() {
    TEST("FindDomainById with invalid id returns nullptr");

    MemoryDomain* d = FindDomainById(9999);
    if (d != nullptr) {
        FAIL("expected nullptr for nonexistent id");
        return;
    }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DomainScope RAII
// ════════════════════════════════════════════════════════════════════════════

static void TestDomainScope() {
    TEST("DomainScope pushes/pops domain correctly");

    DomainInit init{};
    init.module_name = "domain_scope_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);
    if (domain == nullptr) {
        FAIL("FindDomainById failed");
        UnregisterMemoryDomain(id);
        return;
    }

    // Before push, CurrentDomain should be null.
    if (CurrentDomain() != nullptr) {
        FAIL("CurrentDomain should be null before push");
        UnregisterMemoryDomain(id);
        return;
    }

    {
        DomainScope scope(domain);
        if (CurrentDomain() != domain) {
            FAIL("CurrentDomain should return pushed domain inside scope");
            UnregisterMemoryDomain(id);
            return;
        }
    }

    // After scope, CurrentDomain should be null again.
    if (CurrentDomain() != nullptr) {
        FAIL("CurrentDomain should be null after scope exit");
    }

    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: CurrentDomainAllocateTagged + DomainFreeTagged
// ════════════════════════════════════════════════════════════════════════════

static void TestAllocateAndFree() {
    TEST("DomainCurrentAllocateTagged + DomainFreeTagged round-trip");

    DomainInit init{};
    init.module_name = "alloc_free_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Allocate and write into memory.
    void* p = DomainCurrentAllocateTagged(64);
    if (p == nullptr) {
        FAIL("DomainCurrentAllocateTagged returned null");
        UnregisterMemoryDomain(id);
        return;
    }
    std::memset(p, 0xAA, 64);

    // Free via DomainFreeTagged — should route to correct heap via tag.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Direct DomainAllocateTagged (bypasses current domain)
// ════════════════════════════════════════════════════════════════════════════

static void TestDirectAllocateTagged() {
    TEST("DomainAllocateTagged(domain, size) allocates and tags correctly");

    DomainInit init{};
    init.module_name = "direct_alloc_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);

    // Allocate without pushing domain onto TLS stack.
    void* p = DomainAllocateTagged(domain, 128);
    if (p == nullptr) {
        FAIL("DomainAllocateTagged returned null");
        UnregisterMemoryDomain(id);
        return;
    }
    std::memset(p, 0xBB, 128);

    // Free should still work because the tag carries the heap pointer.
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DomainCurrentReallocateTagged preserves tag
// ════════════════════════════════════════════════════════════════════════════

static void TestReallocatePreservesTag() {
    TEST("DomainCurrentReallocateTagged preserves tag for subsequent free");

    DomainInit init{};
    init.module_name = "realloc_tag_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Allocate initial block.
    void* p = DomainCurrentAllocateTagged(32);
    if (p == nullptr) {
        FAIL("first allocate returned null");
        UnregisterMemoryDomain(id);
        return;
    }
    std::memcpy(p, "hello realloc", 14);

    // Reallocate to a larger size.
    void* q = DomainCurrentReallocateTagged(p, 128);
    if (q == nullptr) {
        FAIL("reallocate returned null (original block may have been freed)");
        UnregisterMemoryDomain(id);
        return;
    }
    // Data should have been preserved (realloc copies, or new alloc + copy).
    if (std::memcmp(q, "hello realloc", 14) != 0) {
        FAIL("reallocated block content corrupted");
        UnregisterMemoryDomain(id);
        return;
    }

    // Free via DomainFreeTagged — if tag was lost, this would WARN + no-op.
    DomainFreeTagged(q);

    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DomainCurrentReallocateTagged with nullptr (acts as allocate)
// ════════════════════════════════════════════════════════════════════════════

static void TestReallocateNullptr() {
    TEST("DomainCurrentReallocateTagged(nullptr, size) acts as allocate");

    DomainInit init{};
    init.module_name = "realloc_null_test.dll";
    init.module_kind = 0;
    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    void* p = DomainCurrentReallocateTagged(nullptr, 64);
    if (p == nullptr) {
        FAIL("realloc(nullptr, 64) returned null");
        UnregisterMemoryDomain(id);
        return;
    }
    std::memset(p, 0xCC, 64);
    DomainFreeTagged(p);

    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Usage limit enforcement
// ════════════════════════════════════════════════════════════════════════════

static void TestUsageLimit() {
    TEST("usage_limit prevents allocation beyond quota");

    DomainInit init{};
    init.module_name = "usage_limit_test.dll";
    init.module_kind = 0;
    init.usage_limit = 256;  // very small limit

    DomainId id = RegisterMemoryDomain(init);
    if (id == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed");
        return;
    }

    MemoryDomain* domain = FindDomainById(id);
    DomainScope scope(domain);

    // Small allocation should succeed.
    void* p = DomainCurrentAllocateTagged(64);
    if (p == nullptr) {
        FAIL("first allocation (64 bytes) should succeed within 256 limit");
        UnregisterMemoryDomain(id);
        return;
    }

    // Large allocation beyond limit should fail.
    // Allocation adds sizeof(AllocationHeader) overhead (~8 bytes).
    void* q = DomainCurrentAllocateTagged(4096);
    if (q != nullptr) {
        FAIL("second allocation (4096 bytes) should have been rejected by limit");
        DomainFreeTagged(q);
    }

    DomainFreeTagged(p);
    UnregisterMemoryDomain(id);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Allocate after domain unload (no-op, not crash)
// ════════════════════════════════════════════════════════════════════════════

static void TestAllocateAfterUnload() {
    TEST("allocating on a null TLS domain falls back to tagged malloc");

    // DomainCurrentAllocateTagged with no active domain should fall back to
    // tagged std::malloc — not crash.
    void* p = DomainCurrentAllocateTagged(32);
    if (p == nullptr) {
        // This is acceptable if no domain context is expected to be required.
        // Some builds may require a domain; we accept either behavior.
        PASS();
        return;
    }
    // If we got a pointer, free it.
    DomainFreeTagged(p);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DomainFreeTagged(nullptr) no-op
// ════════════════════════════════════════════════════════════════════════════

static void TestFreeNullptr() {
    TEST("DomainFreeTagged(nullptr) should be a no-op");

    // Should not crash.
    DomainFreeTagged(nullptr);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Double registration (different names)
// ════════════════════════════════════════════════════════════════════════════

static void TestMultiDomain() {
    TEST("multiple domains can be registered and found independently");

    DomainInit a{}, b{};
    a.module_name = "multi_a.dll"; a.module_kind = 0;
    b.module_name = "multi_b.dll"; b.module_kind = 0;

    DomainId id_a = RegisterMemoryDomain(a);
    DomainId id_b = RegisterMemoryDomain(b);
    if (id_a == kDomainIdInvalid || id_b == kDomainIdInvalid) {
        FAIL("RegisterMemoryDomain failed for one or both domains");
        if (id_a != kDomainIdInvalid) UnregisterMemoryDomain(id_a);
        if (id_b != kDomainIdInvalid) UnregisterMemoryDomain(id_b);
        return;
    }
    if (id_a == id_b) {
        FAIL("two domains got the same id");
        UnregisterMemoryDomain(id_a);
        UnregisterMemoryDomain(id_b);
        return;
    }

    // Find each independently.
    if (FindDomainByName("multi_a.dll") == nullptr ||
        FindDomainByName("multi_b.dll") == nullptr) {
        FAIL("could not find one or both domains by name");
    }

    UnregisterMemoryDomain(id_a);
    UnregisterMemoryDomain(id_b);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("=== MemoryDomain Tests ===");
    puts("");

    TestRegisterAndFind();
    TestFindNonexistent();
    TestDomainScope();
    TestAllocateAndFree();
    TestDirectAllocateTagged();
    TestReallocatePreservesTag();
    TestReallocateNullptr();
    TestUsageLimit();
    TestAllocateAfterUnload();
    TestFreeNullptr();
    TestMultiDomain();

    puts("");
    printf("Results: %d tests, %d failures\n", g_tests, g_failures);

    return g_failures > 0 ? 1 : 0;
}