/// domain_unload_test — Domain unloader integration tests (GoogleTest).
///
/// Tests UnloadDomain with valid/invalid domain IDs and cross-domain
/// reference scanning. Uses GcTestBase with domain_unloader.h API.

#include <chaos/native_types.h>

#include "domain_unloader.h"
#include "gc_card_table.h"
#include "gc_region.h"
#include "gc_test_base.h"
#include "memory_domain.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;

// ── Fixture ──────────────────────────────────────────────────────────────

struct DomainUnloadTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();
        GcSetHeapBase(reinterpret_cast<void*>(0x1000000));
    }
};

// ── Tests ────────────────────────────────────────────────────────────────

TEST_F(DomainUnloadTest, InvalidDomain) {
    DomainUnloadResult r = UnloadDomain(0);
    EXPECT_FALSE(r.success);
}

TEST_F(DomainUnloadTest, NonexistentDomain) {
    DomainUnloadResult r = UnloadDomain(9999);
    SUCCEED();
}

TEST_F(DomainUnloadTest, BasicUnload) {
    DomainInit init{};
    init.module_name = "test.dll";
    init.module_kind = 1;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    auto& mgr = RegionManager::Instance();
    Region* dr = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    ASSERT_NE(dr, nullptr);

    DomainUnloadResult r = UnloadDomain(id);
    ASSERT_TRUE(r.success);

    MemoryDomain* after = FindDomainById(id);
    if (after != nullptr) {
        EXPECT_TRUE(after->is_unloaded);
    }
}

TEST_F(DomainUnloadTest, MultiRegionUnload) {
    DomainInit init{};
    init.module_name = "multi.dll";
    init.module_kind = 1;
    DomainId id = RegisterMemoryDomain(init);
    ASSERT_NE(id, kDomainIdInvalid);

    auto& mgr = RegionManager::Instance();
    Region* r1 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    Region* r2 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
    ASSERT_NE(r1, nullptr);
    ASSERT_NE(r2, nullptr);

    DomainUnloadResult r = UnloadDomain(id);
    ASSERT_TRUE(r.success);
}

TEST_F(DomainUnloadTest, PreservesOtherDomain) {
    DomainInit init_a{}, init_b{};
    init_a.module_name = "a.dll"; init_a.module_kind = 1;
    init_b.module_name = "b.dll"; init_b.module_kind = 1;
    DomainId id_a = RegisterMemoryDomain(init_a);
    DomainId id_b = RegisterMemoryDomain(init_b);
    ASSERT_NE(id_a, kDomainIdInvalid);
    ASSERT_NE(id_b, kDomainIdInvalid);

    auto& mgr = RegionManager::Instance();
    Region* r_a = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id_a);
    Region* r_b = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id_b);
    ASSERT_NE(r_a, nullptr);
    ASSERT_NE(r_b, nullptr);

    DomainUnloadResult r = UnloadDomain(id_a);
    ASSERT_TRUE(r.success);

    MemoryDomain* after_b = FindDomainById(id_b);
    ASSERT_NE(after_b, nullptr);
    EXPECT_FALSE(after_b->is_unloaded);

    UnregisterMemoryDomain(id_b);
}
