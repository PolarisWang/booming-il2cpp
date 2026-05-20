/// gc_bgc_stress_test — BGC stress test (GoogleTest).
///
/// 5 stress scenarios: concurrent allocation, work-stealing tree walk,
/// many threads, finalization, and graph mutation with SATB barriers.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_bgc.h"
#include "gc_bgc_inline.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
#include "gc_helpers.h"
#include "gc_card_table.h"
#include "thread_state.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

namespace {

// ── Type infrastructure ───────────────────────────────────────────

static constexpr uint64_t kFakeStableId = 0x1000000000000001ULL;

struct alignas(8) FakeTypeInfo {
    uint64_t stable_id = kFakeStableId;
    uint64_t _pad[3] = {};
};

static FakeTypeInfo g_fake_type_info;

static void InitCrossGen() {
    GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(64);
    uintptr_t addr = reinterpret_cast<uintptr_t>(&g_fake_type_info);
    GcLayoutRegistry::Instance().RegisterTypeInfoRange(addr, addr + sizeof(FakeTypeInfo));
}

// ── Graph mutation structures ─────────────────────────────────────

struct alignas(32) GraphTypeInfo {
    uint64_t stable_id;
    uint64_t _pad1;
    uint64_t stable_id2;
    uint64_t _pad2;
};

struct GraphNode {
    GraphTypeInfo* type_info;
    uint64_t sync_state;
    GraphNode* next;
    uint64_t payload[5];
};

// ── Tree structures ──────────────────────────────────────────────

struct TreeNode {
    void* type_info;
    uint64_t sync_state;
    TreeNode* left;
    TreeNode* right;
    uint64_t payload[4];
};

static constexpr uint16_t kTreePtrOffsets[] = { offsetof(TreeNode, left),
                                                 offsetof(TreeNode, right) };

// ── Helpers ───────────────────────────────────────────────────────

static bool WaitForPhase(BgcPhase phase, int timeout_ms = 120000) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline) {
        if (BgcController::Instance().Phase() == phase) return true;
        threading::SafepointPoll();
        std::this_thread::yield();
    }
    return false;
}

static void* AllocOldGen(size_t size, bool scanning = true) {
    return g_old_gen.Allocate(size, scanning);
}

static void RunBgcCycle() {
    while (BgcController::Instance().IsBusy())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::REMARK_NEEDED));

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::COMPACT_NEEDED));

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    BgcController::Instance().WaitForCycleComplete();
}

// ── Finalizer ────────────────────────────────────────────────────

static std::atomic<int> g_finalizer_count{0};
static void TestFinalizer(void* obj) {
    g_finalizer_count.fetch_add(1, std::memory_order_relaxed);
}

// ── Fixture ──────────────────────────────────────────────────────

struct BgcStressTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();
        if (!s_inited.load()) {
            GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
            g_old_gen.Init(0, 2);
            InitCrossGen();
            BgcController::Instance().Start();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            s_inited.store(true);
        }
    }
    void TearDown() override {
        GcStressTestBase::TearDown();
    }

    static std::atomic<bool> s_inited;
};

std::atomic<bool> BgcStressTest::s_inited{false};

} // anonymous namespace

// ── Test 1: Concurrent stress ─────────────────────────────────────

TEST_F(BgcStressTest, ConcurrentStress) {
    static constexpr int kThreads = 4;
    static constexpr int kRootsPerThread = 20;
    static constexpr int kGarbagePerCycle = 500;
    static constexpr int kCycles = 2;

    std::atomic<bool> failed{false};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        std::vector<void*> roots;
        roots.reserve(kRootsPerThread);

        for (int c = 0; c < kCycles; c++) {
            for (int i = 0; i < kRootsPerThread; i++) {
                void* obj = AllocOldGen(64);
                if (!obj) { failed.store(true); break; }
                std::memset(obj, 0xAB, 64);
                *static_cast<FakeTypeInfo**>(obj) = &g_fake_type_info;
                roots.push_back(obj);
            }
            for (int i = 0; i < kGarbagePerCycle; i++) {
                volatile void* tmp = AllocOldGen(48 + (i % 16) * 4);
                (void)tmp;
            }
            if (failed.load()) break;
            if (c < kCycles - 1)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(worker, t);

    for (int c = 0; c < kCycles; c++) {
        while (BgcController::Instance().IsBusy())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        RunBgcCycle();
    }

    for (auto& th : threads) th.join();
    ASSERT_FALSE(failed.load()) << "Concurrent stress failed";
    SUCCEED();
}

// ── Test 2: Work-stealing tree walk ──────────────────────────────

TEST_F(BgcStressTest, WorkStealingStress) {
    static constexpr int kDepth = 8;  // 255 nodes

    // Register TreeNode layout with left/right pointer offsets.
    uint64_t tree_sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(sizeof(TreeNode));
    GcLayoutRegistry::Instance().Register(tree_sid, sizeof(TreeNode), kTreePtrOffsets, 2);

    auto build_tree = [&](int depth, auto&& self) -> TreeNode* {
        if (depth == 0) return nullptr;
        void* mem = AllocOldGen(sizeof(TreeNode));
        if (!mem) return nullptr;
        auto* node = static_cast<TreeNode*>(mem);
        std::memset(node, 0, sizeof(TreeNode));
        node->type_info = const_cast<void*>(
            reinterpret_cast<const void*>(&g_fake_type_info));
        node->left = self(depth - 1, self);
        node->right = self(depth - 1, self);
        // Touch payload to ensure pages are committed.
        node->payload[0] = 0xCAFEBABE;
        return node;
    };

    TreeNode* tree = build_tree(kDepth, build_tree);
    ASSERT_NE(tree, nullptr);

    // Walk tree with volatile locals to prevent optimization.
    std::atomic<int> walk_sum{0};
    auto walk = [&](auto&& self, TreeNode* n) -> void {
        if (!n) return;
        walk_sum.fetch_add(1);
        self(self, n->left);
        self(self, n->right);
    };

    // Run BGC while tree references exist.
    RunBgcCycle();

    // Verify tree is still accessible.
    walk(walk, tree);
    EXPECT_EQ(walk_sum.load(), (1 << kDepth) - 1);
    SUCCEED();
}

// ── Test 3: Many threads ─────────────────────────────────────────

TEST_F(BgcStressTest, ManyThreads) {
    static constexpr int kThreads = 4;
    static constexpr int kAllocsPerThread = 200;
    std::atomic<bool> failed{false};

    auto worker = [&](int) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kAllocsPerThread; i++) {
            void* obj = AllocOldGen(48 + (i % 16) * 4);
            if (!obj) { failed.store(true); break; }
            std::memset(obj, 0xCD, 48 + (i % 16) * 4);
            if ((i % 50) == 0) threading::SafepointPoll();
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++)
        threads.emplace_back(worker, t);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    RunBgcCycle();

    for (auto& th : threads) th.join();
    ASSERT_FALSE(failed.load());
    SUCCEED();
}

// ── Test 4: Finalization stress ──────────────────────────────────

TEST_F(BgcStressTest, FinalizationStress) {
    static constexpr int kNumObjects = 200;
    g_finalizer_count.store(0);

    std::vector<void*> objs;
    for (int i = 0; i < kNumObjects; i++) {
        void* obj = AllocOldGen(64);
        ASSERT_NE(obj, nullptr);
        g_old_gen.RegisterFinalizer(obj, TestFinalizer);
        objs.push_back(obj);
    }
    objs.clear();

    RunBgcCycle();
    chaos_gc_wait_for_pending_finalizers();

    EXPECT_GT(g_finalizer_count.load(), 0) << "At least some finalizers should have run";
}

// ── Test 5: Graph mutation ──────────────────────────────────────

TEST_F(BgcStressTest, GraphMutation) {
    static constexpr int kNumNodes = 500;
    std::vector<GraphNode*> nodes;

    // Build linked list.
    GraphNode* prev = nullptr;
    for (int i = 0; i < kNumNodes; i++) {
        void* mem = AllocOldGen(sizeof(GraphNode));
        ASSERT_NE(mem, nullptr);
        auto* node = static_cast<GraphNode*>(mem);
        std::memset(node, 0, sizeof(GraphNode));
        node->type_info = reinterpret_cast<GraphTypeInfo*>(&g_fake_type_info);
        node->next = prev;
        g_old_gen.AddPinnedRoot(node, sizeof(GraphNode));
        nodes.push_back(node);
        prev = node;
    }

    // Mutator thread: rewrite next pointers with SATB barriers.
    std::atomic<bool> mutator_done{false};
    std::thread mutator([&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        while (!mutator_done.load()) {
            for (size_t i = 0; i < nodes.size(); i++) {
                size_t target = (i + 1) % nodes.size();
                BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&nodes[i]->next));
                nodes[i]->next = nodes[target];
                DirtyCard(&nodes[i]->next);
            }
            threading::SafepointPoll();
        }
        threading::UnregisterThread();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    RunBgcCycle();

    mutator_done.store(true);
    mutator.join();

    // Verify all nodes survived.
    for (auto* node : nodes) {
        ASSERT_TRUE(g_old_gen.IsInOldGen(node)) << "Node lost during BGC graph mutation";
        EXPECT_EQ(node->type_info, reinterpret_cast<GraphTypeInfo*>(&g_fake_type_info));
    }
    SUCCEED();
}
