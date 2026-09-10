/// loh_stress_test — Large Object Heap stress test (GoogleTest).
///
/// 5 scenarios converted from old contracts test.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_loh.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_stats.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

static void WritePattern(void* p, CHAOS_IL2CPP_SIZE size, int thread_id, int iter) {
    auto* buf = static_cast<uint8_t*>(p);
    buf[0] = static_cast<uint8_t>(thread_id);
    buf[1] = static_cast<uint8_t>(iter);
    buf[2] = 0xAB;
    buf[3] = 0xCD;
    for (CHAOS_IL2CPP_SIZE i = 4; i < size; i++)
        buf[i] = static_cast<uint8_t>((thread_id + iter + i) & 0xFF);
}

static bool VerifyPattern(const void* p, CHAOS_IL2CPP_SIZE size, int thread_id, int iter) {
    auto* buf = static_cast<const uint8_t*>(p);
    if (buf[0] != static_cast<uint8_t>(thread_id)) return false;
    if (buf[1] != static_cast<uint8_t>(iter)) return false;
    if (buf[2] != 0xAB || buf[3] != 0xCD) return false;
    for (CHAOS_IL2CPP_SIZE i = 4; i < size; i++)
        if (buf[i] != static_cast<uint8_t>((thread_id + iter + i) & 0xFF)) return false;
    return true;
}

struct LohStressTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();
        if (!s_old_gen_inited.load()) {
            g_old_gen.Init(0, 2);
            s_old_gen_inited.store(true);
        }
    }

    static std::atomic<bool> s_old_gen_inited;
};

std::atomic<bool> LohStressTest::s_old_gen_inited{false};

} // anonymous namespace

TEST_F(LohStressTest, L1_BasicStress) {
    static constexpr int kThreads = 8;
    static constexpr int kAllocs = 50;  // reduced from 500 for stress label
    std::atomic<bool> failed{false};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kAllocs; i++) {
            CHAOS_IL2CPP_SIZE size = 86 * 1024 + (tid * 17 + i * 31) % (512 * 1024 - 86 * 1024);
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if ((i % 200) == 0) threading::SafepointPoll();
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();
    threads.clear();

    ASSERT_FALSE(failed.load()) << "allocation failed";

    g_old_gen.Collect(nullptr, nullptr);

    std::atomic<int> verified{0};
    auto verifier = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < 10; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if (!VerifyPattern(obj, size, tid, i)) { failed.store(true); break; }
            verified.fetch_add(1);
        }
        threading::UnregisterThread();
    };

    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(verifier, t);
    for (auto& th : threads) th.join();

    ASSERT_FALSE(failed.load()) << "pattern verification failed";
    SUCCEED();
}

TEST_F(LohStressTest, L2_SweepStress) {
    static constexpr int kThreads = 4;
    static constexpr int kAllocs = 50;
    std::atomic<bool> failed{false};
    std::mutex all_roots_mutex;
    std::vector<void*> all_roots;

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kAllocs; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if (i % 2 == 0) {
                std::lock_guard<std::mutex> lock(all_roots_mutex);
                all_roots.push_back(obj);
            }
            if ((i % 100) == 0) threading::SafepointPoll();
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();
    ASSERT_FALSE(failed.load()) << "sweep stress allocation failed";

    for (auto* r : all_roots)
        g_loh.MarkObject(r);
    g_loh.Sweep();

    for (auto* r : all_roots)
        ASSERT_TRUE(g_loh.IsInLOH(r)) << "root lost after sweep";
    SUCCEED();
}

TEST_F(LohStressTest, L3_FreeListReuse) {
    static constexpr int kRounds = 50;

    for (int r = 0; r < kRounds; r++) {
        CHAOS_IL2CPP_SIZE size = 96 * 1024 + (r % 4) * 8192;
        void* obj = g_loh.Allocate(size);
        ASSERT_NE(obj, nullptr) << "allocation failed";
        memset(obj, 0xAA, size);

        g_loh.Free(obj);

        void* obj2 = g_loh.Allocate(size);
        ASSERT_NE(obj2, nullptr) << "re-allocation failed";

        bool zeroed = true;
        for (CHAOS_IL2CPP_SIZE i = 0; i < size && i < 1024; i++) {
            if (static_cast<uint8_t*>(obj2)[i] != 0) { zeroed = false; break; }
        }
        ASSERT_TRUE(zeroed) << "reused segment not zeroed";

        if ((r % 20) == 0) threading::SafepointPoll();
    }
    SUCCEED();
}

TEST_F(LohStressTest, L4_CompactStress) {
    static constexpr int kAllocs = 200;
    std::atomic<bool> failed{false};
    std::mutex shared_mutex;
    std::vector<void*> all_keep;
    std::vector<void*> all_free;

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kAllocs; i++) {
            CHAOS_IL2CPP_SIZE size = 128 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            {
                std::lock_guard<std::mutex> lock(shared_mutex);
                if (i % 2 == 0) all_keep.push_back(obj);
                else all_free.push_back(obj);
            }
            if ((i % 200) == 0) threading::SafepointPoll();
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();
    ASSERT_FALSE(failed.load()) << "allocation failed";

    for (auto* f : all_free)
        g_loh.Free(f);
    for (auto* k : all_keep)
        g_loh.MarkObject(k);
    g_loh.Sweep();

    for (auto* k : all_keep)
        ASSERT_TRUE(g_loh.IsInLOH(k)) << "keep lost after compaction";
    SUCCEED();
}

TEST_F(LohStressTest, L5_MixedPressure) {
    static constexpr int kLohThreads = 4;
    static constexpr int kOldGenThreads = 2;
    static constexpr int kLohAllocs = 50;
    static constexpr int kOldGenAllocs = 100;

    std::atomic<bool> failed{false};
    std::atomic<int> loh_ok{0};
    std::atomic<int> oldgen_ok{0};

    auto loh_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kLohAllocs; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024 + (tid * 13 + i * 7) % (256 * 1024);
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if (!VerifyPattern(obj, size, tid, i)) { failed.store(true); break; }
            if ((i % 100) == 0) threading::SafepointPoll();
        }
        loh_ok.fetch_add(1);
        threading::UnregisterThread();
    };

    auto oldgen_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kOldGenAllocs; i++) {
            void* obj = g_old_gen.Allocate(64 + (tid * 8 + i * 3) % 1024, true);
            if (!obj) { failed.store(true); break; }
            if ((i % 200) == 0) threading::SafepointPoll();
        }
        oldgen_ok.fetch_add(1);
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kLohThreads; t++)
        threads.emplace_back(loh_worker, t);
    for (int t = 0; t < kOldGenThreads; t++)
        threads.emplace_back(oldgen_worker, t + kLohThreads);
    for (auto& th : threads) th.join();

    ASSERT_FALSE(failed.load()) << "mixed pressure failed";
    SUCCEED();
}
