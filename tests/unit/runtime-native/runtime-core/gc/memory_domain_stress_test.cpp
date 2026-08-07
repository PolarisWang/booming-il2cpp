/// MemoryDomain stress/soak test.
///
/// Exercises domain API in high-frequency loops:
///   - Register/Find/Unregister x 1000
///   - DomainCurrentAllocateTagged/DomainFreeTagged x 10000
///   - DomainScope push/pop x 1000
///   - Cross-domain alloc/free x 1000
///   - FindDomainByName x 1000

#include <chaos/native_types.h>

#include "memory_domain.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::memory_domain;

static constexpr const char* kModuleName   = "stress_test.dll";
static constexpr CHAOS_IL2CPP_UINT32 kHotUpdateKind = 1u;

// ═══════════════════════════════════════════════════════════════════════════
// Test: Register+Unregister 1000 times
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainStress, RegisterUnregister_1000Iterations) {
    for (int i = 0; i < 1000; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        DomainInit init{};
        init.module_name = kModuleName;
        init.module_kind = kHotUpdateKind;
        init.usage_limit = 0;

        DomainId id = RegisterMemoryDomain(init);
        ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed at iter " << i;

        MemoryDomain* found = FindDomainById(id);
        ASSERT_NE(found, nullptr) << "FindDomainById returned null at iter " << i;
        EXPECT_EQ(found->domain_id, id) << "domain_id mismatch at iter " << i;

        MemoryDomain* by_name = FindDomainByName(kModuleName);
        EXPECT_EQ(by_name, found) << "FindDomainByName mismatch at iter " << i;

        bool ok = UnregisterMemoryDomain(id);
        ASSERT_TRUE(ok) << "UnregisterMemoryDomain failed at iter " << i;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Allocate+Free 10000 times
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainStress, AllocateFree_10000Iterations) {
    DomainInit init{};
    init.module_name = "alloc_free_stress.dll";
    init.module_kind = kHotUpdateKind;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr) << "FindDomainById failed";
    DomainScope scope(domain);

    for (int i = 0; i < 10000; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        void* p = DomainCurrentAllocateTagged(64);
        ASSERT_NE(p, nullptr) << "DomainCurrentAllocateTagged returned null at iter " << i;
        std::memset(p, 0xAA, 64);
        DomainFreeTagged(p);
    }

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: DomainScope push/pop 1000 times
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainStress, DomainScope_1000Iterations) {
    DomainInit init{};
    init.module_name = "scope_stress.dll";
    init.module_kind = kHotUpdateKind;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    MemoryDomain* domain = FindDomainById(id);
    ASSERT_NE(domain, nullptr) << "FindDomainById failed";

    for (int i = 0; i < 1000; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        EXPECT_EQ(CurrentDomain(), nullptr)
            << "CurrentDomain should be null before push at iter " << i;
        {
            DomainScope scope(domain);
            EXPECT_EQ(CurrentDomain(), domain)
                << "CurrentDomain should be pushed domain inside scope at iter " << i;
        }
        EXPECT_EQ(CurrentDomain(), nullptr)
            << "CurrentDomain should be null after scope exit at iter " << i;
    }

    UnregisterMemoryDomain(id);
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: Cross-domain alloc/free 1000 times
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainStress, CrossDomainAllocFree_1000Iterations) {
    static constexpr const char* kDomains[] = {
        "cross_a.dll", "cross_b.dll", "cross_c.dll"
    };
    static constexpr int kNumDomains = 3;

    DomainId ids[kNumDomains];
    MemoryDomain* domains[kNumDomains];

    for (int d = 0; d < kNumDomains; d++) {
        DomainInit init{};
        init.module_name = kDomains[d];
        init.module_kind = kHotUpdateKind;
        init.usage_limit = 0;
        ids[d] = RegisterMemoryDomain(init);
        ASSERT_NE(ids[d], kDomainIdInvalid) << "RegisterMemoryDomain failed for domain " << d;
        domains[d] = FindDomainById(ids[d]);
        ASSERT_NE(domains[d], nullptr) << "FindDomainById failed for domain " << d;
    }

    for (int i = 0; i < 1000; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        int domain_idx = i % kNumDomains;

        DomainScope scope(domains[domain_idx]);
        EXPECT_EQ(CurrentDomain(), domains[domain_idx])
            << "Wrong current domain at iter " << i;

        void* p = DomainCurrentAllocateTagged(32);
        ASSERT_NE(p, nullptr) << "Allocation failed at iter " << i;
        std::memset(p, static_cast<int>(0xBB + domain_idx), 32);

        // Free should correctly route to the originating heap via tag.
        DomainFreeTagged(p);
    }

    for (int d = 0; d < kNumDomains; d++) {
        UnregisterMemoryDomain(ids[d]);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Test: FindDomainByName 1000 times
// ═══════════════════════════════════════════════════════════════════════════

TEST(MemoryDomainStress, FindByName_1000Iterations) {
    DomainInit init{};
    init.module_name = "findbyname_stress.dll";
    init.module_kind = kHotUpdateKind;
    init.usage_limit = 0;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid) << "RegisterMemoryDomain failed";

    for (int i = 0; i < 1000; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        MemoryDomain* by_name = FindDomainByName("findbyname_stress.dll");
        ASSERT_NE(by_name, nullptr) << "FindDomainByName returned null at iter " << i;
        EXPECT_EQ(by_name->domain_id, id) << "domain_id mismatch at iter " << i;
    }

    UnregisterMemoryDomain(id);
}
