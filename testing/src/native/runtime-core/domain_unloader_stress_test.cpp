/// domain_unloader_stress_test — DomainUnloader 压力测试 (GoogleTest).
///
/// Stress tests for UnloadDomain under concurrent access, high-frequency
/// register/unload cycles, and multi-domain scenarios.
///
/// Level: ⭐⭐⭐ 压力测试补完 (Loader 工业化 Phase 1)
///
/// Follows patterns from memory_domain_stress_test.cpp and
/// gc/domain_unload_test.cpp.

#include <chaos/native_types.h>

#include "domain_unloader.h"
#include "gc_card_table.h"
#include "gc_region.h"
#include "gc_test_base.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;

// ── Fixture ──────────────────────────────────────────────────────────────────

struct DomainUnloaderStressTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();
        GcSetHeapBase(reinterpret_cast<void*>(0x1000000));
    }

    /// Register a domain with a single region.
    DomainId CreateDomainWithRegion(const char* name, uint32_t kind = 1) {
        DomainInit init{};
        init.module_name = name;
        init.module_kind = kind;
        init.usage_limit = 0;
        DomainId id = RegisterMemoryDomain(init);
        if (id == kDomainIdInvalid) return id;

        auto& mgr = RegionManager::Instance();
        Region* dr = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
        if (dr == nullptr) {
            UnregisterMemoryDomain(id);
            return kDomainIdInvalid;
        }
        return id;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Test: Register + Unload 循环 100 次
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, RegisterUnload_100Iterations) {
    for (int i = 0; i < 100; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));
        DomainId id = CreateDomainWithRegion("stress_loop.dll");
        ASSERT_NE(id, kDomainIdInvalid) << "CreateDomainWithRegion failed at iter " << i;

        DomainUnloadResult r = UnloadDomain(id);
        ASSERT_TRUE(r.success) << "UnloadDomain failed at iter " << i;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: 多区域 domain — 每 domain 分配 5 个 region，然后 unload
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, MultiRegionUnload_50Iterations) {
    for (int i = 0; i < 50; i++) {
        SCOPED_TRACE("iteration " + std::to_string(i));

        DomainInit init{};
        init.module_name = "multi_region.dll";
        init.module_kind = 1;
        init.usage_limit = 0;
        DomainId id = RegisterMemoryDomain(init);
        ASSERT_NE(id, kDomainIdInvalid);

        auto& mgr = RegionManager::Instance();
        for (int r = 0; r < 5; r++) {
            Region* dr = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
            ASSERT_NE(dr, nullptr) << "AllocateRegion failed at iter " << i << " region " << r;
        }

        DomainUnloadResult r = UnloadDomain(id);
        ASSERT_TRUE(r.success);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: N 个 domain 依次 unload — 验证不互相影响
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, ManyDomains_SequentialUnload) {
    static constexpr int kDomainCount = 50;

    DomainId ids[kDomainCount];
    for (int i = 0; i < kDomainCount; i++) {
        std::string name = "domain_" + std::to_string(i) + ".dll";
        ids[i] = CreateDomainWithRegion(name.c_str());
        ASSERT_NE(ids[i], kDomainIdInvalid) << "CreateDomainWithRegion failed for domain " << i;
    }

    // Unload in reverse order to stress ordering edge cases.
    for (int i = kDomainCount - 1; i >= 0; i--) {
        SCOPED_TRACE("unloading domain " + std::to_string(i));
        DomainUnloadResult r = UnloadDomain(ids[i]);
        ASSERT_TRUE(r.success);

        MemoryDomain* after = FindDomainById(ids[i]);
        if (after != nullptr) {
            EXPECT_TRUE(after->is_unloaded);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: 相同 module name 复用 — register → unload → re-register
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, DomainReuse_SameModuleName) {
    static constexpr int kReuseCount = 20;

    for (int i = 0; i < kReuseCount; i++) {
        SCOPED_TRACE("reuse iteration " + std::to_string(i));

        DomainId id = CreateDomainWithRegion("reusable.dll");
        ASSERT_NE(id, kDomainIdInvalid);
        EXPECT_NE(id, kDomainIdInvalid);

        DomainUnloadResult r = UnloadDomain(id);
        ASSERT_TRUE(r.success);

        // After unload, the domain should not be findable by name.
        MemoryDomain* by_name = FindDomainByName("reusable.dll");
        EXPECT_EQ(by_name, nullptr)
            << "Domain should not be findable after unload at iter " << i;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: 并发 register + unload — 4 线程各 25 次
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, ConcurrentRegisterUnload_4Threads) {
    static constexpr int kThreadCount = 4;
    static constexpr int kIterPerThread = 25;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int t = 0; t < kThreadCount; t++) {
        int thread_id = t;
        threads.emplace_back([&failures, thread_id]() {
            int32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);

            for (int i = 0; i < kIterPerThread; i++) {
                std::string name = "concurrent_" + std::to_string(thread_id) + "_" + std::to_string(i) + ".dll";

                DomainInit init{};
                init.module_name = name.c_str();
                init.module_kind = 1;
                init.usage_limit = 0;
                DomainId id = RegisterMemoryDomain(init);
                if (id == kDomainIdInvalid) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }

                auto& mgr = RegionManager::Instance();
                Region* dr = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, id);
                if (dr == nullptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                    UnregisterMemoryDomain(id);
                    continue;
                }

                // Write a pattern to the region to simulate domain memory usage.
                std::memset(dr->begin, 0xCD, 64);

                DomainUnloadResult r = UnloadDomain(id);
                if (!r.success) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }

            threading::UnregisterThread();
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0) << "Concurrent register/unload had failures";
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: Unload domain 后 domain_id 可被重用
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, DomainIdReuseAfterUnload) {
    DomainId first_id = CreateDomainWithRegion("first.dll");
    ASSERT_NE(first_id, kDomainIdInvalid);

    DomainUnloadResult r = UnloadDomain(first_id);
    ASSERT_TRUE(r.success);

    // Register again — the implementation should be able to reuse the id.
    DomainId second_id = CreateDomainWithRegion("second.dll");
    ASSERT_NE(second_id, kDomainIdInvalid);

    // It's valid for second_id to differ from first_id; we just verify
    // that registration still works after an unload cycle.
    DomainUnloadResult r2 = UnloadDomain(second_id);
    ASSERT_TRUE(r2.success);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: Unload domain 有其他 domain 活跃 — 验证不影响其他 domain
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, UnloadDoesNotAffectOtherDomains) {
    DomainId survivor = CreateDomainWithRegion("survivor.dll");
    ASSERT_NE(survivor, kDomainIdInvalid);

    // Create and unload many domains while keeping survivor alive.
    for (int i = 0; i < 30; i++) {
        SCOPED_TRACE("victim iteration " + std::to_string(i));
        std::string name = "victim_" + std::to_string(i) + ".dll";
        DomainId victim = CreateDomainWithRegion(name.c_str());
        ASSERT_NE(victim, kDomainIdInvalid);

        DomainUnloadResult r = UnloadDomain(victim);
        ASSERT_TRUE(r.success);
    }

    // Survivor should still be valid.
    MemoryDomain* after = FindDomainById(survivor);
    ASSERT_NE(after, nullptr);
    EXPECT_FALSE(after->is_unloaded);

    // Clean up survivor.
    DomainUnloadResult r = UnloadDomain(survivor);
    ASSERT_TRUE(r.success);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Test: 同时多个线程 unload 不同 domain
// ═══════════════════════════════════════════════════════════════════════════════
TEST_F(DomainUnloaderStressTest, ConcurrentUnloadDifferentDomains) {
    static constexpr int kDomainCount = 20;
    std::atomic<int> failures{0};

    DomainId ids[kDomainCount];
    for (int i = 0; i < kDomainCount; i++) {
        std::string name = "concurrent_unload_" + std::to_string(i) + ".dll";
        ids[i] = CreateDomainWithRegion(name.c_str());
        ASSERT_NE(ids[i], kDomainIdInvalid);
    }

    // Spawn 4 threads, each unloading 5 different domains.
    std::vector<std::thread> threads;
    threads.reserve(4);
    for (int t = 0; t < 4; t++) {
        int start_idx = t * 5;
        threads.emplace_back([&failures, &ids, start_idx]() {
            int32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);

            for (int i = start_idx; i < start_idx + 5 && i < kDomainCount; i++) {
                DomainUnloadResult r = UnloadDomain(ids[i]);
                if (!r.success) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }

            threading::UnregisterThread();
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0) << "Concurrent unload of different domains had failures";
}
