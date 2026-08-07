/// module_registry_test.cpp — ModuleRegistry 单元 + 并发测试 (GoogleTest).
///
/// 覆盖 RegisterModule / LookupModule / LookupModuleByName /
/// MarkModuleTombstone / IsModuleTombstone / GetModuleCount /
/// GetModuleByIndex / TypeInfoHandle utilities / TokenToIndex
/// 以及 concurrent read/write 安全。
///
/// Level: ⭐⭐ 测试 + 文档化 (Loader 工业化 Phase 1)

#include <module_registry.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::runtime_core;

// ── Test helpers ────────────────────────────────────────────────────────────

/// Create a minimal ModuleDescriptor with a given name.
static ModuleDescriptor MakeDescriptor(const char* name) {
    ModuleDescriptor desc{};
    desc.name_utf8 = name;
    desc.type_count = 0;
    return desc;
}

// ═══════════════════════════════════════════════════════════════════════════
// TokenToIndex
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, TokenToIndex_TypeDef_ZeroBased) {
    EXPECT_EQ(TokenToIndex(0x02000001u), 0u);  // first type → index 0
    EXPECT_EQ(TokenToIndex(0x02000005u), 4u);  // 5th type → index 4
    EXPECT_EQ(TokenToIndex(0x02000000u), 0u);  // edge case: zero raw index
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoHandle encode/decode
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, TypeInfoHandle_RoundTrip) {
    uint32_t module_id = 42;
    uint32_t token = 0x02000007u;
    TypeInfoHandle handle = MakeTypeHandle(module_id, token);
    EXPECT_EQ(GetModuleId(handle), module_id);
    EXPECT_EQ(GetTypeToken(handle), token);
}

TEST(module_registry_test, TypeInfoHandle_ZeroModule) {
    TypeInfoHandle handle = MakeTypeHandle(0, 0x02000001u);
    EXPECT_EQ(GetModuleId(handle), 0u);
    EXPECT_EQ(GetTypeToken(handle), 0x02000001u);
}

// ═══════════════════════════════════════════════════════════════════════════
// RegisterModule / LookupModule
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, RegisterModule_Basic) {
    auto desc = MakeDescriptor("Test.Module.dll");
    uint32_t id = RegisterModule("Test.Module.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);
    EXPECT_GE(id, 1u);  // [0] = CoreLib reserved

    const auto* looked = LookupModule(id);
    ASSERT_NE(looked, nullptr);
    EXPECT_STREQ(looked->name_utf8, "Test.Module.dll");
}

TEST(module_registry_test, RegisterModule_NullName_ReturnsInvalid) {
    auto desc = MakeDescriptor("unused");
    EXPECT_EQ(RegisterModule(nullptr, &desc), kInvalidModuleId);
}

TEST(module_registry_test, RegisterModule_NullDescriptor_ReturnsInvalid) {
    EXPECT_EQ(RegisterModule("Test.dll", nullptr), kInvalidModuleId);
}

TEST(module_registry_test, RegisterModule_MultipleModules_UniqueIds) {
    auto desc_a = MakeDescriptor("A.dll");
    auto desc_b = MakeDescriptor("B.dll");
    auto desc_c = MakeDescriptor("C.dll");

    uint32_t id_a = RegisterModule("A.dll", &desc_a);
    uint32_t id_b = RegisterModule("B.dll", &desc_b);
    uint32_t id_c = RegisterModule("C.dll", &desc_c);

    ASSERT_NE(id_a, kInvalidModuleId);
    ASSERT_NE(id_b, kInvalidModuleId);
    ASSERT_NE(id_c, kInvalidModuleId);

    EXPECT_NE(id_a, id_b);
    EXPECT_NE(id_b, id_c);
    EXPECT_NE(id_a, id_c);
}

// ═══════════════════════════════════════════════════════════════════════════
// LookupModule edge cases
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, LookupModule_InvalidId_ReturnsNull) {
    EXPECT_EQ(LookupModule(kInvalidModuleId), nullptr);
    EXPECT_EQ(LookupModule(kMaxModules), nullptr);     // out of bounds
    EXPECT_EQ(LookupModule(kMaxModules + 100), nullptr); // far out of bounds
}

TEST(module_registry_test, LookupModule_UnregisteredId_ReturnsNull) {
    // Module IDs start at 1, an unregistered high ID should be null.
    EXPECT_EQ(LookupModule(9999), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// LookupModuleByName
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, LookupModuleByName_Found) {
    auto desc = MakeDescriptor("FindByName.dll");
    uint32_t id = RegisterModule("FindByName.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    const auto* found = LookupModuleByName("FindByName.dll");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found, LookupModule(id));
}

TEST(module_registry_test, LookupModuleByName_NotFound_ReturnsNull) {
    EXPECT_EQ(LookupModuleByName("Nonexistent.Module.dll"), nullptr);
}

TEST(module_registry_test, LookupModuleByName_Null_ReturnsNull) {
    EXPECT_EQ(LookupModuleByName(nullptr), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// MarkModuleTombstone / IsModuleTombstone — lifecycle
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, Tombstone_Basic) {
    auto desc = MakeDescriptor("ToUnload.dll");
    uint32_t id = RegisterModule("ToUnload.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    EXPECT_FALSE(IsModuleTombstone(id));

    MarkModuleTombstone(id);

    EXPECT_TRUE(IsModuleTombstone(id));
}

TEST(module_registry_test, Tombstone_InvalidModuleId_NoOp) {
    // Should not crash.
    MarkModuleTombstone(kInvalidModuleId);
    MarkModuleTombstone(kMaxModules);
    MarkModuleTombstone(0);  // CoreLib reserved — no-op
}

TEST(module_registry_test, Tombstone_LookupByName_AfterTombstone) {
    auto desc = MakeDescriptor("Gone.dll");
    uint32_t id = RegisterModule("Gone.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    MarkModuleTombstone(id);

    // Tombstone modules are skipped in name lookup.
    EXPECT_EQ(LookupModuleByName("Gone.dll"), nullptr);
}

TEST(module_registry_test, Tombstone_ModuleReuse) {
    auto desc = MakeDescriptor("Reusable.dll");
    uint32_t id1 = RegisterModule("Reusable.dll", &desc);
    ASSERT_NE(id1, kInvalidModuleId);

    MarkModuleTombstone(id1);

    // Register again — should reuse the tombstoned slot.
    auto desc2 = MakeDescriptor("Replacement.dll");
    uint32_t id2 = RegisterModule("Replacement.dll", &desc2);
    ASSERT_NE(id2, kInvalidModuleId);

    const auto* looked = LookupModule(id2);
    ASSERT_NE(looked, nullptr);
    EXPECT_STREQ(looked->name_utf8, "Replacement.dll");
    EXPECT_FALSE(IsModuleTombstone(id2));

    // The old name should no longer be findable.
    EXPECT_EQ(LookupModuleByName("Reusable.dll"), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// GetModuleCount / GetModuleByIndex
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, GetModuleCount_AtLeastOne) {
    // Slot 0 (CoreLib fallback) always present.
    EXPECT_GE(GetModuleCount(), 1u);
}

TEST(module_registry_test, GetModuleByIndex_CoreLibSlot) {
    const auto* mod = GetModuleByIndex(0);
    ASSERT_NE(mod, nullptr);
    // Slot 0 is the CoreLib fallback — name may be null.
}

TEST(module_registry_test, GetModuleByIndex_OutOfRange_ReturnsNull) {
    EXPECT_EQ(GetModuleByIndex(kMaxModules), nullptr);
}

TEST(module_registry_test, GetModuleByIndex_AfterRegistration) {
    auto desc = MakeDescriptor("IndexTest.dll");
    uint32_t id = RegisterModule("IndexTest.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    const auto* by_index = GetModuleByIndex(id);
    ASSERT_NE(by_index, nullptr);
    EXPECT_STREQ(by_index->name_utf8, "IndexTest.dll");
}

// ═══════════════════════════════════════════════════════════════════════════
// Tombstone → GetModuleByIndex 排除墓碑
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, GetModuleByIndex_SkipsTombstones) {
    auto desc = MakeDescriptor("SkipMe.dll");
    uint32_t id = RegisterModule("SkipMe.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    MarkModuleTombstone(id);

    // GetModuleByIndex should return nullptr for tombstoned modules.
    EXPECT_EQ(GetModuleByIndex(id), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// 并发只读 — 多线程同时 LookupModule / LookupModuleByName / GetModuleCount
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, Concurrent_ReadOnly_NoCrash) {
    // First register a module so we have something to look up.
    auto desc = MakeDescriptor("ConcurrentRead.dll");
    uint32_t id = RegisterModule("ConcurrentRead.dll", &desc);
    ASSERT_NE(id, kInvalidModuleId);

    static constexpr int kThreadCount = 8;
    static constexpr int kIterPerThread = 500;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([id, &failures]() {
            for (int i = 0; i < kIterPerThread; i++) {
                auto* by_id = LookupModule(id);
                if (by_id == nullptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }

                auto* by_name = LookupModuleByName("ConcurrentRead.dll");
                if (by_name == nullptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }

                // GetModuleCount should never be 0.
                if (GetModuleCount() == 0) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// 并发 Tombstone + Register — 验证 shared_mutex 正确保护
// ═══════════════════════════════════════════════════════════════════════════

TEST(module_registry_test, Concurrent_TombstoneAndRegister) {
    static constexpr int kThreadCount = 4;
    static constexpr int kModulesPerThread = 20;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&failures, t]() {
            for (int i = 0; i < kModulesPerThread; i++) {
                std::string name = "concurrent_tomb_" +
                    std::to_string(t) + "_" + std::to_string(i) + ".dll";
                auto desc = MakeDescriptor(name.c_str());
                uint32_t id = RegisterModule(name.c_str(), &desc);
                if (id == kInvalidModuleId) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }

                // Lookup while marking tombstone concurrently (other threads).
                MarkModuleTombstone(id);

                // After tombstone, lookup should fail.
                if (LookupModuleByName(name.c_str()) != nullptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}
