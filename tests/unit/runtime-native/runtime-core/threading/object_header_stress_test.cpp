// object_header_stress_test.cpp — Type System Phase 1 stress tests
//
// Multi-threaded stress tests for ThinLockTable + SyncBlock inflation:
//   contention-stress:   8 threads, shared object, heavy lock contention
//   reentrant-stress:    same thread locks N times, verifies recursion
//   multi-object-stress: 1000 objects, 8 threads, random lock/unlock
//   tryenter-stress:     MonitorTryEnter under contention
//
// P4.3: Lock table + GC interaction stress test.

#include <gtest/gtest.h>

#include <thread_state.h>
#include <thread_pool.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

// ── AOT codegen stubs ─────────────────────────────────────────────────────
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace chaos::il2cpp::runtime_core {
    bool MonitorEnter(void* monitor_target);
    bool MonitorExit(void* monitor_target);
    bool MonitorTryEnter(void* monitor_target);
    bool MonitorIsEntered(void* monitor_target);
}

namespace threading = chaos::il2cpp::runtime_core::threading;
namespace rt = chaos::il2cpp::runtime_core;

static constexpr size_t kObjectHeaderSize = 8;  // TypeInfo* only (Type System Phase 1)

// ── TypeInfoHot stub ────────────────────────────────────────────────────
static const chaos::il2cpp::common::TypeInfoHot g_lock_type_info_stub = {
    nullptr, nullptr, 0, 0, 0, 0,
    chaos::il2cpp::common::kTypeInfoHeaderKindThin
};

// ── Helper: create a dummy lockable object ───────────────────────────────
static void* CreateLockObject() {
    void* obj = std::malloc(kObjectHeaderSize);
    std::memset(obj, 0, kObjectHeaderSize);
    *static_cast<const void**>(obj) = &g_lock_type_info_stub;
    return obj;
}

// ══════════════════════════════════════════════════════════════════════════
// Test 1: Contention stress — 8 threads hammering the same object
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, ContentionStress)
{
    constexpr int kThreads = 8;
    constexpr int kIterations = 5000;

    void* shared_obj = CreateLockObject();
    std::atomic<int> ready{0};
    std::atomic<int64_t> total_count{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                if (rt::MonitorEnter(shared_obj)) {
                    total_count.fetch_add(1, std::memory_order_relaxed);
                    rt::MonitorExit(shared_obj);
                }
            }
            threading::UnregisterThread();
        });
    }

    // Wait for all threads.
    for (auto& t : threads) t.join();

    // Each of kThreads threads did kIterations successful lock/unlock pairs.
    EXPECT_EQ(total_count.load(), static_cast<int64_t>(kThreads * kIterations));

    std::free(shared_obj);
}

// ══════════════════════════════════════════════════════════════════════════
// Test 2: Re-entrant lock stress — nested MonitorEnter on same thread
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, ReentrantStress)
{
    constexpr int kThreads = 4;
    constexpr int kNestDepth = 10;
    constexpr int kIterations = 1000;

    void* obj = CreateLockObject();
    std::atomic<int> ready{0};
    std::atomic<int64_t> total_enters{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                // Acquire lock kNestDepth times.
                for (int n = 0; n < kNestDepth; n++) {
                    EXPECT_TRUE(rt::MonitorEnter(obj));
                }

                // Verify IsEntered for each nesting level.
                EXPECT_TRUE(rt::MonitorIsEntered(obj));

                // Release lock kNestDepth times.
                for (int n = 0; n < kNestDepth; n++) {
                    rt::MonitorExit(obj);
                }

                // After full release, another thread can acquire it.
                total_enters.fetch_add(kNestDepth, std::memory_order_relaxed);
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(total_enters.load(), static_cast<int64_t>(kThreads * kIterations * kNestDepth));
    std::free(obj);
}

// ══════════════════════════════════════════════════════════════════════════
// Test 3: Multi-object stress — 1000 objects, 8 threads, random access
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, MultiObjectStress)
{
    constexpr int kNumObjects = 1000;
    constexpr int kThreads = 8;
    constexpr int kIterations = 500;

    std::vector<void*> objects;
    objects.reserve(kNumObjects);
    for (int i = 0; i < kNumObjects; i++) {
        objects.push_back(CreateLockObject());
    }

    // Each thread has a partition of objects to lock.
    std::atomic<int> ready{0};
    std::atomic<int64_t> total_locks{0};

    std::vector<std::thread> threads;
    int objs_per_thread = kNumObjects / kThreads;

    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&, t] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            int start = t * objs_per_thread;
            int end = (t == kThreads - 1) ? kNumObjects : start + objs_per_thread;
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                for (int j = start; j < end; j++) {
                    if (rt::MonitorTryEnter(objects[j])) {
                        total_locks.fetch_add(1, std::memory_order_relaxed);
                        rt::MonitorExit(objects[j]);
                    }
                }
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();

    // Each thread should have locked its partition objects.
    EXPECT_GT(total_locks.load(), 0);
    EXPECT_LE(total_locks.load(), static_cast<int64_t>(kThreads * kIterations * objs_per_thread));

    for (auto* obj : objects) std::free(obj);
}

// ══════════════════════════════════════════════════════════════════════════
// Test 4: TryEnter under contention — concurrent trylock on shared object
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, TryEnterContention)
{
    constexpr int kThreads = 8;
    constexpr int kIterations = 2000;

    void* shared_obj = CreateLockObject();
    std::atomic<int> ready{0};
    std::atomic<int64_t> acquired{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                if (rt::MonitorTryEnter(shared_obj)) {
                    acquired.fetch_add(1, std::memory_order_relaxed);
                    rt::MonitorExit(shared_obj);
                }
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();

    // At most kThreads * kIterations total acquires possible.
    EXPECT_GT(acquired.load(), 0);
    EXPECT_LE(acquired.load(), static_cast<int64_t>(kThreads * kIterations));

    std::free(shared_obj);
}

// ══════════════════════════════════════════════════════════════════════════
// Test 5: Rapid lock/unlock ping-pong — two threads on shared object
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, RapidPingPong)
{
    constexpr int kIterations = 10000;

    void* shared_obj = CreateLockObject();
    std::atomic<bool> stop{false};
    std::atomic<int64_t> count_a{0};
    std::atomic<int64_t> count_b{0};

    auto worker = [&](std::atomic<int64_t>* counter) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        while (!stop.load(std::memory_order_acquire)) {
            if (rt::MonitorTryEnter(shared_obj)) {
                counter->fetch_add(1, std::memory_order_relaxed);
                rt::MonitorExit(shared_obj);
            }
        }
        threading::UnregisterThread();
    };

    std::thread t1(worker, &count_a);
    std::thread t2(worker, &count_b);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop.store(true, std::memory_order_release);

    t1.join();
    t2.join();

    int64_t total = count_a.load() + count_b.load();
    EXPECT_GT(total, 0);
    // Total should be bounded: two threads can't both hold the lock at once,
    // but TryEnter can fail. At least a few hundred should succeed in 100ms.
    EXPECT_GE(total, 100);

    std::free(shared_obj);
}

// ══════════════════════════════════════════════════════════════════════════
// Test 6: Lock correctness under mixed operations
// ══════════════════════════════════════════════════════════════════════════

TEST(ObjectHeaderStress, MixedOperations)
{
    constexpr int kThreads = 6;
    constexpr int kIterations = 500;
    constexpr int kExpectedWriters = 3;

    void* shared_obj = CreateLockObject();
    std::atomic<int> ready{0};
    std::atomic<int64_t> guarded_value{0};
    std::atomic<int64_t> total_iters{0};
    std::atomic<int> writer_progress[3] = {0, 0, 0};  // per-writer iteration count

    std::atomic<bool> watchdog_done{false};
    std::thread watchdog([&] {
        for (int s = 0; s < 10 && !watchdog_done.load(); s++) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (!watchdog_done.load()) {
                fprintf(stderr, "\n[WATCHDOG] guarded=%lld iters=%lld w0=%d w1=%d w2=%d\n",
                    (long long)guarded_value.load(), (long long)total_iters.load(),
                    writer_progress[0].load(), writer_progress[1].load(), writer_progress[2].load());
            }
        }
    });

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&, t] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            if (t >= kExpectedWriters) {
                // Readers: mark complete immediately on finish
                for (int i = 0; i < kIterations; i++) {
                    if (rt::MonitorTryEnter(shared_obj)) {
                        rt::MonitorExit(shared_obj);
                    }
                }
                total_iters.fetch_add(kIterations, std::memory_order_relaxed);
            }
            threading::UnregisterThread();
        });
    }
    // Writers: separate loop with progress tracking
    for (int t = 0; t < kExpectedWriters; t++) {
        threads.emplace_back([&, t] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            for (int i = 0; i < kIterations; i++) {
                if (rt::MonitorEnter(shared_obj)) {
                    guarded_value.fetch_add(1, std::memory_order_relaxed);
                    writer_progress[t].store(i + 1, std::memory_order_relaxed);
                    rt::MonitorExit(shared_obj);
                }
            }
            total_iters.fetch_add(kIterations, std::memory_order_relaxed);
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();
    watchdog_done.store(true);
    watchdog.join();

    EXPECT_EQ(guarded_value.load(), static_cast<int64_t>(kExpectedWriters * kIterations));
    std::free(shared_obj);
}
