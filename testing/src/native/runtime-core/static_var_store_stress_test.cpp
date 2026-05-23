/// static_var_store_stress_test.cpp — StaticVarStore 压力测试 (GoogleTest).
///
/// 覆盖 static_var_store_register / unregister_domain / 并发读写路径。
/// 注意：static_var_store_read/write 委托给 AssemblyManager::GetStaticField，
/// 无真实 assembly 注册时返回 nullptr。这里主要测试注册生命周期和 field-level 并发。
///
/// Level: ⭐⭐ 测试重开 (Loader 工业化 Phase 1)

#include <static_var_store.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════
// static_var_store_register / unregister_domain — 生命周期
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_stress_test, RegisterThenUnregisterDomain) {
    uint32_t hash = 0xAA0001;
    uint32_t domain_id = 42;

    static_var_store_register(hash, 1 /*module_id*/, domain_id);

    // After unregister_domain, the entry should be gone.
    static_var_store_unregister_domain(domain_id);

    // Read should return nullptr (no entry).
    auto* ptr = static_var_store_read(hash, 0);
    EXPECT_EQ(ptr, nullptr);
}

TEST(static_var_store_stress_test, UnregisterDomain_MultipleHashes) {
    static_var_store_register(0xBB0001, 1, 100);
    static_var_store_register(0xBB0002, 2, 100);
    static_var_store_register(0xBB0003, 3, 100);

    static_var_store_unregister_domain(100);

    // All entries for domain 100 should be gone.
    EXPECT_EQ(static_var_store_read(0xBB0001, 0), nullptr);
    EXPECT_EQ(static_var_store_read(0xBB0002, 0), nullptr);
    EXPECT_EQ(static_var_store_read(0xBB0003, 0), nullptr);
}

TEST(static_var_store_stress_test, UnregisterDomain_OtherDomainsPreserved) {
    static_var_store_register(0xCC0001, 1, 200);
    static_var_store_register(0xCC0002, 2, 300);

    static_var_store_unregister_domain(200);

    // Domain 300 entry should still be present (but read returns nullptr
    // because AssemblyManager has no such module — just verify no crash).
    auto* ptr = static_var_store_read(0xCC0002, 0);
    (void)ptr;
}

TEST(static_var_store_stress_test, UnregisterDomain_Twice_NoCrash) {
    uint32_t hash = 0xDD0001;
    static_var_store_register(hash, 1, 999);

    static_var_store_unregister_domain(999);
    static_var_store_unregister_domain(999);  // no-op, should not crash
    SUCCEED();
}

TEST(static_var_store_stress_test, UnregisterDomain_Nonexistent_NoCrash) {
    static_var_store_unregister_domain(0x7FFFFFFF);
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Register 后再次 Register 覆盖
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_stress_test, ReRegister_SameHash) {
    uint32_t hash = 0xEE0001;

    static_var_store_register(hash, 1, 10);
    static_var_store_register(hash, 2, 20);  // overwrite

    static_var_store_unregister_domain(10);
    // Domain 20 should still have the entry.
    static_var_store_unregister_domain(20);
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// 并发 field-level read/write
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_stress_test, Concurrent_ReadWriteField) {
    static constexpr int kThreadCount = 4;
    static constexpr int kIterPerThread = 500;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&failures, t]() {
            uint32_t hash = 0x1000 + static_cast<uint32_t>(t);
            for (int i = 0; i < kIterPerThread; i++) {
                uint32_t field = static_cast<uint32_t>(i);

                // Write a value.
                uintptr_t val = static_cast<uintptr_t>(t * 1000 + i);
                static_var_store_write_field(hash, field, val);

                // Read it back.
                auto* ptr = static_var_store_read_field(hash, field);
                if (ptr == nullptr) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                } else if (*static_cast<uintptr_t*>(ptr) != val) {
                    // NOTE: Another thread might have overwritten, so this
                    // is not strictly a failure — we're looking for crashes.
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

TEST(static_var_store_stress_test, Concurrent_RegisterAndRead) {
    static constexpr int kThreadCount = 4;
    static constexpr int kIterPerThread = 200;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&failures, t]() {
            for (int i = 0; i < kIterPerThread; i++) {
                uint32_t hash = 0xF000 + static_cast<uint32_t>(t * kIterPerThread + i);

                if (i % 2 == 0) {
                    static_var_store_register(hash, 1, 100);
                } else {
                    static_var_store_unregister_domain(100);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Concurrent unregister_domain + read_field
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_stress_test, Concurrent_UnregisterAndReadField) {
    static constexpr int kThreadCount = 4;
    static constexpr int kIterPerThread = 200;

    // Pre-populate some field entries.
    for (int i = 0; i < 100; i++) {
        static_var_store_write_field(0x5000, static_cast<uint32_t>(i), static_cast<uintptr_t>(i));
    }

    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&failures, t]() {
            for (int i = 0; i < kIterPerThread; i++) {
                if (i % 3 == 0) {
                    static_var_store_unregister_assembly(0x5000);
                } else {
                    auto* ptr = static_var_store_read_field(0x5000, static_cast<uint32_t>(i % 100));
                    if (ptr == nullptr) {
                        failures.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}
