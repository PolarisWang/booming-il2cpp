/// assembly_manager_stress_test — AssemblyManager 并发压力测试 (GoogleTest).
///
/// 针对 Loader 工业化评估中发现的锁粒度不一致和边界不严谨问题，
/// 覆盖以下场景：
///   1. 并发只读查询（FindAssembly / FindByModuleId / GetStaticField / LoadedCount）
///   2. NextAlcId 并发单调递增
///   3. 锁粒度验证：
///      LoadAssembly 内部持有 table_mutex，但 UnloadAssembly / FindAssembly /
///      FindByModuleId / GetStaticField 均未加锁，存在读写竞争
///   4. 重复 Unload 空安全
///
/// Level: ⭐⭐ 测试 + 锁粒度验证 (Loader 工业化 Phase 1)

#include <assembly_manager.h>
#include <domain_unloader.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>
#include <csetjmp>
#include <string>
#include <thread>
#include <vector>

// ── Stub external symbols ──────────────────────────────────────────────────
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace chaos::il2cpp::runtime_core {
thread_local jmp_buf* g_chaos_exception_jmp = nullptr;
}

namespace gc = chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// 并发只读查询 — FindAssembly / FindByModuleId / GetStaticField 多线程同时调用
//
// 这些方法均无锁，验证并发调用不会导致崩溃或状态损坏。
// 结果（null/非null）取决于竞态，但绝不应 segfault。
// ═══════════════════════════════════════════════════════════════════════════
TEST(assembly_manager_stress_test, ConcurrentReadOnly_DoesNotCrash) {
    auto* mgr = gc::AssemblyManager::Get();

    static constexpr int kThreadCount = 8;
    static constexpr int kIterPerThread = 1000;
    std::atomic<int> seen_errors{0};

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([mgr, &seen_errors, t]() {
            for (int i = 0; i < kIterPerThread; i++) {
                // Phase 1: FindAssembly with various names.
                // The AssemblyManager is empty, so all should return null,
                // but we test that concurrent iteration doesn't corrupt state.
                auto* found = mgr->FindAssembly("nonexistent.dll");
                (void)found;

                found = mgr->FindAssembly("Another.Assembly");
                (void)found;

                // Phase 2: FindByModuleId.
                auto* by_module = mgr->FindByModuleId(static_cast<uint32_t>(i) + 1);
                (void)by_module;

                by_module = mgr->FindByModuleId(0);
                (void)by_module;

                // Phase 3: GetStaticField.
                auto* field = mgr->GetStaticField(static_cast<uint32_t>(i) + 1, 0);
                (void)field;

                field = mgr->GetStaticField(0, 0);
                (void)field;

                // Phase 4: LoadedCount — should never drop below 0 (uint underflow).
                uint32_t count = mgr->LoadedCount();
                if (count > 10000) {
                    seen_errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(seen_errors.load(), 0) << "LoadedCount returned suspiciously high value";
}

// ═══════════════════════════════════════════════════════════════════════════
// NextAlcId 并发安全 — 多线程同时调用应返回唯一 ID
//
// NextAlcId 使用函数局部 static 计数器，无显式同步。
// 如果 MSVC 对 static int 的线程安全初始化有保证，此测试应通过。
// ═══════════════════════════════════════════════════════════════════════════
TEST(assembly_manager_stress_test, NextAlcId_Concurrent_UniqueIds) {
    static constexpr int kThreadCount = 4;
    static constexpr int kIdsPerThread = 100;
    static constexpr int kTotalIds = kThreadCount * kIdsPerThread;

    std::vector<uint32_t> ids[kThreadCount];
    for (int t = 0; t < kThreadCount; t++) {
        ids[t].reserve(kIdsPerThread);
    }

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([&ids, t]() {
            for (int i = 0; i < kIdsPerThread; i++) {
                ids[t].push_back(gc::AssemblyManager::NextAlcId());
            }
        });
    }

    for (auto& th : threads) th.join();

    // Collect all IDs and check for duplicates.
    std::vector<uint32_t> all_ids;
    all_ids.reserve(kTotalIds);
    for (int t = 0; t < kThreadCount; t++) {
        all_ids.insert(all_ids.end(), ids[t].begin(), ids[t].end());
    }

    // Sort and check for uniqueness.
    std::sort(all_ids.begin(), all_ids.end());
    auto dup = std::adjacent_find(all_ids.begin(), all_ids.end());
    EXPECT_EQ(dup, all_ids.end()) << "Duplicate NextAlcId found: " << *dup;

    // Check monotonicity.
    for (auto it = all_ids.begin() + 1; it != all_ids.end(); ++it) {
        EXPECT_GT(*it, *(it - 1)) << "NextAlcId not monotonic";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// UnloadAssembly 重复调用 — 空安全验证
//
// UnloadAssembly 在 ALC is_loaded=false 或 nullptr 时应返回 false。
// 不会造成二次释放。
// ═══════════════════════════════════════════════════════════════════════════
TEST(assembly_manager_stress_test, UnloadAssembly_DoubleUnload_Safe) {
    auto* mgr = gc::AssemblyManager::Get();

    // nullptr → false
    EXPECT_FALSE(mgr->UnloadAssembly(nullptr));
    EXPECT_FALSE(mgr->UnloadAssembly(nullptr));  // double

    // Also verify LoadedCount unchanged.
    EXPECT_EQ(mgr->LoadedCount(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// 锁粒度边界验证 — 并发只读与 UnloadAssembly 交替
//
// 此测试无法触发竞态崩溃（因无法创建真实 ALC 来真正 unload），
// 但验证在空 AssemblyManager 上交替调用不会崩溃。
// ═══════════════════════════════════════════════════════════════════════════
TEST(assembly_manager_stress_test, ConcurrentFindAndUnload_NoCrash) {
    auto* mgr = gc::AssemblyManager::Get();

    static constexpr int kThreadCount = 4;
    static constexpr int kIterPerThread = 500;
    std::atomic<int> failures{0};

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int t = 0; t < kThreadCount; t++) {
        threads.emplace_back([mgr, &failures, t]() {
            for (int i = 0; i < kIterPerThread; i++) {
                if (i % 2 == 0) {
                    // Even iterations: read-only queries.
                    auto* found = mgr->FindAssembly("test.dll");
                    if (found != nullptr) {
                        // Shouldn't happen in this test, but check defensively.
                        failures.fetch_add(1, std::memory_order_relaxed);
                    }
                    (void)mgr->FindByModuleId(static_cast<uint32_t>(i));
                    (void)mgr->GetStaticField(static_cast<uint32_t>(i), 0);
                    (void)mgr->LoadedCount();
                } else {
                    // Odd iterations: unload (always null/not-loaded → false).
                    if (mgr->UnloadAssembly(nullptr)) {
                        failures.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }
        });
    }

    for (auto& th : threads) th.join();
    EXPECT_EQ(failures.load(), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// AssemblyLoadContext struct 并发访问验证
//
// AssemblyManager 使用 std::vector<AssemblyLoadContext>，按索引访问无锁。
// 主要关注 point of use: 当 slot 被并发 Reset 时，reader 可能读到部分重置。
// 此测试构造局部 ALC 验证 atomic<bool> is_unloading 的正确使用模式。
// ═══════════════════════════════════════════════════════════════════════════
TEST(assembly_manager_stress_test, AtomicIsUnloading_Visibility) {
    gc::AssemblyLoadContext alc{};
    alc.is_loaded = true;
    alc.is_unloading.store(false, std::memory_order_relaxed);

    std::atomic<int> readers_saw_unloading{0};
    std::atomic<bool> writer_done{false};

    // Reader thread: poll is_unloading.
    std::thread reader([&]() {
        while (!writer_done.load(std::memory_order_acquire)) {
            if (alc.is_unloading.load(std::memory_order_acquire)) {
                readers_saw_unloading.fetch_add(1, std::memory_order_relaxed);
            }
        }
    });

    // Wait for reader to start polling before modifying the flag.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Writer thread: set is_unloading.
    alc.is_unloading.store(true, std::memory_order_release);
    // Hold the flag true long enough for a well-timed reader to observe it.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // Simulate teardown work.
    alc.is_unloading.store(false, std::memory_order_relaxed);
    alc.is_loaded = false;

    writer_done.store(true, std::memory_order_release);
    reader.join();

    // The reader should have observed the unloading flag.
    // This is a visibility test, not a performance test.
    EXPECT_GT(readers_saw_unloading.load(), 0)
        << "Reader never observed is_unloading=true — memory ordering may be incorrect";
}
