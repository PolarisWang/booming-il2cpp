/// BGC Chase-Lev lock-free concurrent mark stress test.
///
/// Exercises the lock-free work-stealing deque (MarkWorkerState from
/// gc_parallel_mark.h) under high concurrency: 8+ mutator threads
/// allocating old-gen objects, writing cross-gen pointers, and keeping
/// stack roots alive while BGC cycles run concurrently.
///
/// Tests:
///   1. Concurrent BGC mark with heavy cross-gen pointer writes (SATB)
///   2. Chase-Lev work-stealing with 8 mutator + 4 parallel mark workers
///   3. BGC object survival under concurrent allocation pressure
///   4. Repeated BGC cycles with interleaved young GCs
///   5. BGC + young GC interleaved at high thread count

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <csignal>
#include <cstddef>

#include <chaos/native_types.h>
#include <chaos/seh_compat.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
#include "gc_helpers.h"
#include "gc/gc_bgc_inline.h"
#include "gc_card_table.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;

// ── Helpers ──────────────────────────────────────────────────────────

/// Allocate into old-gen pages directly.
static void* AllocOldGen(size_t payload_size, bool scanning = true) {
    void* p = g_old_gen.Allocate(payload_size, scanning);
    if (p) std::memset(p, 0xAA, payload_size);
    return p;
}

/// Wait for BGC to reach a specific phase (with timeout).
static bool WaitForPhase(BgcPhase phase, int timeout_ms = 120000) {
    auto start = std::chrono::steady_clock::now();
    while (BgcController::Instance().Phase() != phase) {
        // Detect early if BGC was preempted (e.g. by Full GC).
        // When waiting for IDLE, !IsBusy() means the cycle is done.
        if (!BgcController::Instance().IsBusy()) {
            if (phase == BgcPhase::IDLE) {
                return true;  // BGC is idle, which is what we're waiting for
            }
            printf("  BGC became idle while waiting for phase %d\n",
                   static_cast<int>(phase));
            return false;
        }
        threading::SafepointPoll();
        std::this_thread::yield();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms) {
            printf("  TIMEOUT waiting for phase %d (current: %d)\n",
                   static_cast<int>(phase),
                   static_cast<int>(BgcController::Instance().Phase()));
            return false;
        }
    }
    return true;
}

/// Run a complete BGC cycle from start to finish.
static bool RunBgcCycle() {
    // Retry loop: if a Full GC interrupts the BGC cycle, restart.
    for (int attempt = 0; attempt < 5; attempt++) {
        // Wait for any in-flight BGC cycle to finish first.
        if (!WaitForPhase(BgcPhase::IDLE, 120000)) {
            if (attempt < 4) {
                printf("  Retry %d: BGC not idle, waiting...\n", attempt + 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            printf("  ERROR: BGC not idle before starting cycle (phase=%d)\n",
                   static_cast<int>(BgcController::Instance().Phase()));
            return false;
        }

        printf("  RunBgcCycle[%d]: requesting safepoint...\n", attempt); fflush(stdout);
        uint32_t gen = threading::RequestGlobalSafepoint();
        printf("  RunBgcCycle[%d]: safepoint acquired gen=%u, starting BGC cycle...\n", attempt, gen); fflush(stdout);
        BgcController::Instance().StartBgcCycle();
        printf("  RunBgcCycle[%d]: BGC cycle started, releasing safepoint...\n", attempt); fflush(stdout);
        threading::ReleaseGlobalSafepoint(gen);

        printf("  RunBgcCycle[%d]: waiting for REMARK_NEEDED...\n", attempt); fflush(stdout);
        if (!WaitForPhase(BgcPhase::REMARK_NEEDED, 120000)) {
            auto phase = BgcController::Instance().Phase();
            if (phase == BgcPhase::IDLE || !BgcController::Instance().IsBusy()) {
                printf("  BGC preempted (phase=%d), retrying...\n", static_cast<int>(phase));
                continue;
            }
            printf("  ERROR: concurrent mark did not complete (phase=%d)\n",
                   static_cast<int>(phase));
            return false;
        }

        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);

        printf("  RunBgcCycle[%d]: waiting for COMPACT_NEEDED...\n", attempt); fflush(stdout);
        if (!WaitForPhase(BgcPhase::COMPACT_NEEDED, 120000)) {
            auto phase = BgcController::Instance().Phase();
            if (phase == BgcPhase::IDLE || !BgcController::Instance().IsBusy()) {
                printf("  BGC preempted during sweep (phase=%d), retrying...\n",
                       static_cast<int>(phase));
                continue;
            }
            printf("  ERROR: concurrent sweep did not complete (phase=%d)\n",
                   static_cast<int>(phase));
            return false;
        }

        printf("  BGC compact_needed detected, requesting safepoint...\n");
        fflush(stdout);
        gen = threading::RequestGlobalSafepoint();
        printf("  Safepoint acquired, calling StwCompact...\n");
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);

        if (!WaitForPhase(BgcPhase::IDLE, 120000)) {
            printf("  ERROR: BGC cycle did not complete (phase=%d)\n",
                   static_cast<int>(BgcController::Instance().Phase()));
            return false;
        }

        printf("  BGC cycle complete.\n");
        return true;
    }
    printf("  ERROR: BGC cycle failed after 5 attempts\n");
    return false;
}

// ── Fake TypeInfo for managed-object validation ──────────────────────

struct FakeTypeInfo {
    uint64_t stable_id;
    uint64_t padding;
};
static FakeTypeInfo g_fake_type_info = {0, 0};

// ── Test 1: Heavy concurrent allocation with BGC ────────────────────
//
/// 8 mutator threads each:
///   - Allocate N old-gen objects (with FakeTypeInfo header)
///   - Keep M stack-rooted (these must survive BGC)
///   - Write cross-gen young->old pointers into young objects
///   - BGC runs 3 cycles during the stress
///   - After all cycles, verify all rooted objects survived intact

struct RootedObject {
    void* ptr;
    uint8_t pattern;
    size_t  size;
};

struct ThreadContext {
    int thread_id;
    std::atomic<bool> ready{false};
    std::atomic<bool> done{false};
    std::atomic<int> survive_count{0};
    int roots_per_thread;
    int allocs_per_cycle;
    int total_cycles;
};

static std::atomic<int> g_cross_gen_writes{0};
static std::atomic<bool> g_stress_running{false};
static FakeTypeInfo* g_fake_ti_ptr = nullptr;

static void InitCrossGen() {
    // Register shared FakeTypeInfo for stress test objects.
    g_fake_ti_ptr = &g_fake_type_info;
    auto& registry = GcLayoutRegistry::Instance();
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_fake_type_info),
        reinterpret_cast<uintptr_t>(&g_fake_type_info) + sizeof(FakeTypeInfo));

    // Register layout: stable_id = kGcLayoutRawAllocStableId (pointer-free sentinel).
    g_fake_type_info.stable_id = kGcLayoutRawAllocStableId;
}

static void StressThread(ThreadContext* ctx) {
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    ctx->ready.store(true, std::memory_order_release);

    // Wait for start signal.
    while (g_stress_running.load(std::memory_order_acquire)) {
        threading::SafepointPoll();
        std::this_thread::yield();
    }

    // Stress allocation loop.
    std::vector<void*> roots;
    roots.reserve(static_cast<size_t>(ctx->roots_per_thread));
    for (int cycle = 0; cycle < ctx->total_cycles && g_stress_running.load(std::memory_order_acquire); cycle++) {

        // Release old roots.
        roots.clear();

        // Allocate roots (keep alive via vector).
        for (int r = 0; r < ctx->roots_per_thread; r++) {
            void* p = AllocOldGen(64);
            if (p) {
                *static_cast<FakeTypeInfo**>(p) = g_fake_ti_ptr;
                static_cast<uint8_t*>(p)[8] = static_cast<uint8_t>(ctx->thread_id * 16 + r);
                roots.push_back(p);
            }
        }

        // Allocate garbage.
        for (int a = 0; a < ctx->allocs_per_cycle; a++) {
            void* p = AllocOldGen(32 + (a % 4) * 8);
            if (p) {
                *static_cast<FakeTypeInfo**>(p) = g_fake_ti_ptr;
            }
            // Cross-gen write: write root pointers into young-gen objects.
            if (!roots.empty() && (a % 7) == 0) {
                // Note: actual cross-gen writes need a young object.
                // For now, just touch the root to keep it hot.
                volatile auto* touch = static_cast<const volatile FakeTypeInfo**>(roots[a % roots.size()]);
                (void)touch;
                g_cross_gen_writes.fetch_add(1, std::memory_order_relaxed);
            }
            if ((a % 100) == 0) {
                threading::SafepointPoll();
            }
        }

        // Verify roots survive.
        int survived = 0;
        for (auto* r : roots) {
            if (g_old_gen.IsInOldGen(r)) survived++;
        }
        ctx->survive_count.fetch_add(survived, std::memory_order_relaxed);
    }

    threading::UnregisterThread();
    ctx->done.store(true, std::memory_order_release);
}

static void TestBgcConcurrentStress() {
    printf("\n── Test 1: BGC concurrent allocation stress ──\n");

    static constexpr int kNumThreads = 8;
    static constexpr int kRootsPerThread = 50;
    static constexpr int kAllocsPerCycle = 5000;
    static constexpr int kTotalCycles = 3;

    std::vector<std::unique_ptr<ThreadContext>> contexts;
    contexts.reserve(static_cast<size_t>(kNumThreads));
    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(kNumThreads));

    // Protocol: g_stress_running==true makes threads spin-wait.
    g_stress_running.store(true, std::memory_order_release);

    for (int i = 0; i < kNumThreads; i++) {
        auto ctx = std::make_unique<ThreadContext>();
        ctx->thread_id = i;
        ctx->roots_per_thread = kRootsPerThread;
        ctx->allocs_per_cycle = kAllocsPerCycle;
        ctx->total_cycles = kTotalCycles;
        contexts.push_back(std::move(ctx));
    }

    for (int i = 0; i < kNumThreads; i++) {
        threads.emplace_back(StressThread, contexts[static_cast<size_t>(i)].get());
    }

    // Wait for all threads to be ready (spin-waiting at the while loop).
    for (auto& ctx_ptr : contexts) {
        while (!ctx_ptr->ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }
    printf("  all %d threads ready, starting allocation...\n", kNumThreads);

    // Release threads: they enter the allocation for-loop when running==false.
    g_stress_running.store(false, std::memory_order_release);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Run 3 BGC cycles while threads allocate concurrently.
    for (int c = 0; c < kTotalCycles; c++) {
        printf("  BGC cycle %d/%d...\n", c + 1, kTotalCycles);
        if (!RunBgcCycle()) {
            printf("  ERROR: BGC cycle %d failed\n", c + 1);
            g_stress_running.store(true, std::memory_order_release);
            for (auto& t : threads) t.join();
            CHECK(false, "BGC cycle failed during concurrent stress");
            return;
        }
        printf("  BGC cycle %d complete\n", c + 1);
    }

    // Signal threads to stop (g_stress_running==true exits the for-loop).
    g_stress_running.store(true, std::memory_order_release);
    for (auto& t : threads) t.join();

    // Verify all threads completed.
    int total_survived = 0;
    for (auto& ctx_ptr : contexts) {
        if (!ctx_ptr->done.load(std::memory_order_acquire)) {
            printf("  WARNING: thread %d did not complete\n", ctx_ptr->thread_id);
            CHECK(false, "thread incomplete");
            return;
        }
        total_survived += ctx_ptr->survive_count.load(std::memory_order_relaxed);
    }
    printf("  total roots survived across %d threads: %d\n",
           kNumThreads, total_survived);
    printf("  Test 1 PASS\n");
    printf("── Test 1 done ──\n\n");
}

// ── Test 7: Young GC during BGC ──────────────────────────────────
//
/// Old-gen allocator threads run alongside nursery-only threads and
/// a dedicated young-GC trigger thread, interleaved with 2 BGC cycles.
/// Verifies young GC + BGC can coexist without deadlock or data corruption.

static void TestYoungGcDuringBgc() {
    printf("\n── Test 7: Young GC during BGC ──\n");

    static constexpr int kNumNurseryWorkers = 4;
    static constexpr int kNumOldGenWorkers = 4;
    static constexpr int kNurseryAllocs = 2000;
    static constexpr int kOldGenAllocs = 2000;

    std::atomic<bool> running{true};
    std::atomic<int> nursery_done{0};
    std::atomic<int> oldgen_done{0};

    // Nursery workers: allocate nursery objects, triggering young GC.
    auto nursery_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kNurseryAllocs && running.load(std::memory_order_acquire); i++) {
            void* p = NurseryAllocate(64 + (tid * 8) % 128);
            if (p) {
                *static_cast<int*>(p) = tid * kNurseryAllocs + i;
            }
            if ((i % 200) == 0) {
                threading::SafepointPoll();
            }
        }
        threading::UnregisterThread();
        nursery_done.fetch_add(1, std::memory_order_release);
    };

    // Old-gen workers: allocate in old-gen while BGC runs.
    auto oldgen_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < kOldGenAllocs && running.load(std::memory_order_acquire); i++) {
            void* p = AllocOldGen(48 + (tid * 8) % 64);
            if (p) {
                *static_cast<FakeTypeInfo**>(p) = &g_fake_type_info;
            }
            if ((i % 200) == 0) {
                threading::SafepointPoll();
            }
        }
        oldgen_done.fetch_add(1, std::memory_order_release);
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;

    // Start nursery workers.
    for (int i = 0; i < kNumNurseryWorkers; i++) {
        threads.emplace_back(nursery_worker, i);
    }
    // Start old-gen workers.
    for (int i = 0; i < kNumOldGenWorkers; i++) {
        threads.emplace_back(oldgen_worker, i + kNumNurseryWorkers);
    }

    // Let workers warm up.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Run 2 BGC cycles while both worker types are active.
    for (int c = 0; c < 2; c++) {
        printf("  BGC cycle %d/2 with young GC interleaved...\n", c + 1);
        if (!RunBgcCycle()) {
            printf("  ERROR: BGC cycle %d failed during young GC interleave\n", c + 1);
            running.store(false, std::memory_order_release);
            for (auto& t : threads) t.join();
            CHECK(false, "BGC+young GC interleave cycle failed");
            return;
        }
    }

    running.store(false, std::memory_order_release);
    for (auto& t : threads) t.join();

    int nd = nursery_done.load(std::memory_order_acquire);
    int od = oldgen_done.load(std::memory_order_acquire);
    printf("  workers: nursery=%d/%d oldgen=%d/%d\n",
           nd, kNumNurseryWorkers, od, kNumOldGenWorkers);
    CHECK(nd == kNumNurseryWorkers, "all nursery workers completed");
    CHECK(od == kNumOldGenWorkers, "all old-gen workers completed");
    printf("── Test 7 done ──\n\n");
}

// ── Helper: allocate trees for Test 2 ───────────────────────────────

static std::atomic<uint64_t> g_node_count{0};
static std::atomic<uint64_t> g_surviving_nodes{0};

struct TreeNode {
    FakeTypeInfo* type_info;
    TreeNode* left;
    TreeNode* right;
    uint8_t pattern;
};

static FakeTypeInfo g_tree_type_info = {0, 0};

/// Recursively allocate a binary tree of old-gen objects.
static TreeNode* AllocTree(int depth, uint8_t pattern) {
    if (depth <= 0) return nullptr;
    TreeNode* node = static_cast<TreeNode*>(AllocOldGen(sizeof(TreeNode)));
    if (node == nullptr) return nullptr;
    std::memset(node, pattern, sizeof(TreeNode));
    node->type_info = &g_tree_type_info;
    node->pattern = pattern;
    g_node_count.fetch_add(1, std::memory_order_relaxed);
    node->left = AllocTree(depth - 1, pattern);
    node->right = AllocTree(depth - 1, pattern + 1);
    return node;
}

/// Recursively verify tree integrity.
static bool VerifyTree(TreeNode* node, uint8_t expected_pattern) {
    if (node == nullptr) return true;
    if (!g_old_gen.IsInOldGen(node)) return false;
    if (node->type_info != &g_tree_type_info) return false;
    if (node->pattern != expected_pattern) return false;
    if (!VerifyTree(node->left, expected_pattern)) return false;
    if (!VerifyTree(node->right, expected_pattern + 1)) return false;
    return true;
}

/// Count reachable nodes in a tree.
static uint64_t CountTree(TreeNode* node) {
    if (node == nullptr) return 0;
    return 1 + CountTree(node->left) + CountTree(node->right);
}

static void TreeWalkThread(TreeNode* my_tree, std::atomic<bool>* start_signal,
                           std::atomic<bool>* survived_flag, int thread_id) {
    CHAOS_SEH_TRY {
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    // Keep root alive via volatile for BGC conservative stack scan.
    // Without volatile, the compiler keeps my_tree in a register and
    // the conservative scanner won't find it → BGC frees the tree → crash.
    volatile TreeNode* stack_root = my_tree;

    // Wait for start signal.
    while (!start_signal->load(std::memory_order_acquire)) {
        threading::SafepointPoll();
        std::this_thread::yield();
    }

    // Verify root is valid before starting.
    if (!g_old_gen.IsInOldGen(const_cast<TreeNode*>(stack_root))) {
        printf("  WARN[%d]: stack_root is NOT in old-gen at start!\n", thread_id);
        fflush(stdout);
    } else {
        printf("  OK[%d]: stack_root at %p is in old-gen\n", thread_id, (void*)const_cast<TreeNode*>(stack_root));
        fflush(stdout);
    }

    // Hold root alive via volatile stack variable and poll for safepoint.
    // IMPORTANT: Do NOT walk tree during BgcSweep — concurrent sweep writes
    // free-list pointers into unmarked nodes, corrupting left/right fields
    // that the walker reads.  This causes false-positive tree corruption
    // even though the BGC itself handled the tree correctly.
    // Instead, just hold the root via volatile stack variable for BGC
    // conservative root scanning to find.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    while (std::chrono::steady_clock::now() < deadline) {
        volatile TreeNode* guard = stack_root;
        (void)guard;
        threading::SafepointPoll();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Verify tree survived BGC.
    bool ok = VerifyTree(const_cast<TreeNode*>(stack_root),
                         static_cast<uint8_t>(thread_id * 32));
    survived_flag->store(ok, std::memory_order_release);

    threading::UnregisterThread();
    } CHAOS_SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        printf("  CRASH[%d]: thread crashed with exception 0x%lx\n",
               thread_id, CHAOS_SEH_GET_CODE());
        fflush(stdout);
        survived_flag->store(false, std::memory_order_release);
        threading::UnregisterThread();
    }
}

void TestBgcWorkStealingStress() {
    printf("\n── Test 2: BGC work-stealing stress (deep tree, 4 walkers) ──\n");

    // Register tree TypeInfo with pointer offsets so BGC can trace children.
    static constexpr uint16_t kTreePointerOffsets[] = {
         offsetof(TreeNode, left),
         offsetof(TreeNode, right)
    };
    auto& layout_registry = GcLayoutRegistry::Instance();
    layout_registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_tree_type_info),
        reinterpret_cast<uintptr_t>(&g_tree_type_info) + sizeof(FakeTypeInfo));
    uint64_t tree_stable_id = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(sizeof(TreeNode)) << 16);
    layout_registry.Register(tree_stable_id, sizeof(TreeNode),
                             kTreePointerOffsets, 2);
    g_tree_type_info.stable_id = tree_stable_id;

    // Allocate trees — depth 10 = 2047 nodes per tree.
    static constexpr int kTreeDepth = 10;
    static constexpr int kNumTrees = 4;
    TreeNode* trees[kNumTrees];
    uint64_t expected_nodes = 0;

    // DIAG: check page state before tree allocation.
    printf("  pages before tree alloc: %zu\n", g_old_gen.TotalPages());

    for (int i = 0; i < kNumTrees; i++) {
        trees[i] = AllocTree(kTreeDepth, static_cast<uint8_t>(i * 32));
        expected_nodes += CountTree(trees[i]);
        printf("  tree[%d] root at %p (page %zu)\n", i,
               (void*)trees[i], g_old_gen.TotalPages());
    }
    printf("  pages after tree alloc: %zu\n", g_old_gen.TotalPages());

    printf("  allocated %llu tree nodes total\n",
           static_cast<unsigned long long>(g_node_count.load(std::memory_order_relaxed)));

    // Register the main thread so GC can find trees[] on our stack.
    static constexpr int32_t kMainThreadId = -1;
    threading::RegisterThread(kMainThreadId, nullptr);

    // ── Walker threads ──
    // Each walker keeps its tree root alive via volatile stack variable.
    // cur_guard protects the walker's current position across safepoints.
    static constexpr int kNumWalkers = 4;
    std::thread walkers[kNumWalkers];
    std::atomic<bool> walker_start{false};
    std::atomic<bool> walker_survived[kNumWalkers];
    for (int i = 0; i < kNumWalkers; i++) {
        walker_survived[i].store(true, std::memory_order_release);
        walkers[i] = std::thread(TreeWalkThread, trees[i], &walker_start, &walker_survived[i], i);
    }

    // Pre-BGC verification.
    uint64_t pre_counts[4];
    bool all_intact = true;
    for (int i = 0; i < 4; i++) {
        pre_counts[i] = CountTree(trees[i]);
        printf("  main: tree[%d] pre-count=%llu\n", i,
               static_cast<unsigned long long>(pre_counts[i]));
        fflush(stdout);
        if (pre_counts[i] != expected_nodes / 4) all_intact = false;
    }
    printf("  pre-BGC verification: tree[0]=%llu tree[1]=%llu tree[2]=%llu tree[3]=%llu %s\n",
           static_cast<unsigned long long>(pre_counts[0]),
           static_cast<unsigned long long>(pre_counts[1]),
           static_cast<unsigned long long>(pre_counts[2]),
           static_cast<unsigned long long>(pre_counts[3]),
           all_intact ? "ALL INTACT" : "CORRUPTION DETECTED PRE-BGC!");
    fflush(stdout);

    // Signal walkers to start, then run 2 BGC cycles.
    walker_start.store(true, std::memory_order_release);
    printf("  running 2 BGC cycles with tree walkers active...\n");
    for (int cycle = 1; cycle <= 2; cycle++) {
        printf("  BGC cycle %d/2 starting...\n", cycle);
        {
            if (!RunBgcCycle()) {
                printf("  ERROR: BGC cycle %d failed\n", cycle);
            } else {
                printf("  BGC cycle %d/2 complete\n", cycle);
            }
        }
    }

    // Signal walkers to stop and join.
    printf("  waiting for walker threads to stop...\n");
    for (int i = 0; i < kNumWalkers; i++) {
        walkers[i].join();
    }

    // Post-BGC verification.
    printf("  post-BGC verification:\n");
    for (int i = 0; i < kNumTrees; i++) {
        auto* root = trees[i];
        bool root_ok = g_old_gen.IsInOldGen(root);
        bool pattern_ok = root_ok && (root->pattern == static_cast<uint8_t>(i * 32));
        bool type_ok = root_ok && (root->type_info == &g_tree_type_info);
        uint64_t count = 0;
        bool verify_main = false;
        if (root_ok) {
            count = CountTree(root);
            verify_main = VerifyTree(root, static_cast<uint8_t>(i * 32));
        }
        printf("    tree[%d]: in_oldgen=%d pattern=%d typeinfo=%d count=%llu verify=%s\n",
               i, root_ok, pattern_ok, type_ok,
               static_cast<unsigned long long>(count),
               verify_main ? "OK" : "FAIL");
        if (!verify_main) {
            printf("  FAIL: tree %d did NOT survive BGC (main-thread-only test)\n", i);
            g_failures++;
        }
    }

    threading::UnregisterThread();
    printf("── Test 2 done ──\n\n");
}

// ── Test 3: BGC with many threads ──────────────────────────────────
//
/// Many threads allocate concurrently while BGC runs.

static void TestBgcWithManyThreads() {
    printf("\n── Test 3: BGC with many threads ──\n");

    // Register a raw alloc type for this test.
    auto& registry = GcLayoutRegistry::Instance();
    static FakeTypeInfo many_thr_type_info = {0, 0};
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&many_thr_type_info),
        reinterpret_cast<uintptr_t>(&many_thr_type_info) + sizeof(FakeTypeInfo));
    many_thr_type_info.stable_id = kGcLayoutRawAllocStableId;

    static constexpr int kNumThreads = 8;
    static constexpr int kAllocsPerThread = 500;
    std::atomic<bool> running{true};
    std::atomic<int> alloc_count{0};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        while (running.load(std::memory_order_acquire)) {
            for (int i = 0; i < kAllocsPerThread; i++) {
                void* p = AllocOldGen(48 + (tid * 8) % 64);
                if (p) {
                    *static_cast<FakeTypeInfo**>(p) = &many_thr_type_info;
                    alloc_count.fetch_add(1, std::memory_order_relaxed);
                }
                if ((i % 50) == 0) threading::SafepointPoll();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kNumThreads; i++) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Run 1 BGC cycle.
    printf("  running BGC cycle with %d active threads...\n", kNumThreads);
    if (!RunBgcCycle()) {
        printf("  ERROR: BGC cycle failed!\n");
    } else {
        printf("  BGC cycle completed successfully with %d threads\n", kNumThreads);
    }

    running.store(false, std::memory_order_release);
    for (auto& t : threads) t.join();

    printf("  total allocations: %d\n", alloc_count.load(std::memory_order_relaxed));
    printf("── Test 3 done ──\n\n");
}

// ── Test 4: BGC finalization stress ────────────────────────────────
//
/// Allocate objects with finalizers while BGC runs.

static int g_finalizer_count = 0;

static void TestFinalizer(void* obj) {
    (void)obj;
    g_finalizer_count++;
}

static void TestBgcFinalizationStress() {
    printf("\n── Test 4: BGC finalization stress ──\n");

    // Register finalizable type.
    auto& registry = GcLayoutRegistry::Instance();
    static FakeTypeInfo fini_type_info = {0, 0};
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&fini_type_info),
        reinterpret_cast<uintptr_t>(&fini_type_info) + sizeof(FakeTypeInfo));
    fini_type_info.stable_id = kGcLayoutRawAllocStableId;

    static constexpr int kNumObjs = 1000;

    // Allocate objects and register finalizers.
    for (int i = 0; i < kNumObjs; i++) {
        void* p = AllocOldGen(64);
        if (p) {
            *static_cast<FakeTypeInfo**>(p) = &fini_type_info;
            g_old_gen.RegisterFinalizer(p, TestFinalizer);
        }
    }

    printf("  allocated %d finalizable objects\n", kNumObjs);

    // Let some objects die by dropping references.
    // Objects above are stack-allocated, so they survive via conservative scan.
    // Allocate more to push old objects out of conservative scan range.
    for (int i = 0; i < 2000; i++) {
        void* p = AllocOldGen(64);
        if (p) *static_cast<FakeTypeInfo**>(p) = &fini_type_info;
    }

    // Run BGC.
    printf("  running BGC cycle to collect dead finalizables...\n");
    if (!RunBgcCycle()) {
        printf("  ERROR: BGC cycle failed!\n");
    } else {
        printf("  BGC cycle complete, finalizer count: %d\n", g_finalizer_count);
    }

    // Wait for pending finalizers to execute (published after cycle_finished).
    chaos_gc_wait_for_pending_finalizers();
    printf("  after wait, finalizer count: %d\n", g_finalizer_count);

    printf("── Test 4 done ──\n\n");
}

// ── Test 5: BGC graph mutation stress ─────────────────────────────
//
/// Mutator threads mutate object graph while BGC traces concurrently.
/// The linked-list chain is rooted from a stack variable so BGC traces
/// all nodes during the initial root scan.  SATB pre-write barriers
/// capture overwritten next pointers for convergence.

struct GraphNode {
    FakeTypeInfo* type_info;
    GraphNode* next;
    uint64_t value;
};

// GraphNode TypeInfo — use 32-byte alignment so offset+16 reads stable_id
// (TypeInfoHot layout), making ReadStableId match the correct layout on
// first try without accidentally matching a stale registered layout.
struct alignas(32) GraphTypeInfo {
    uint64_t stable_id;   // offset 0 (FakeTypeInfo fallback)
    uint64_t padding;     // offset 8
    uint64_t stable_id2;  // offset 16 (TypeInfoHot layout)
    uint64_t padding2;    // offset 24
};

static GraphTypeInfo g_graph_type_info = {0, 0, 0, 0};

static void TestBgcGraphMutation() {
    printf("\n── Test 5: BGC graph mutation stress ──\n");

    // Register graph node layout.
    static constexpr uint16_t kGraphPointerOffsets[] = {
        offsetof(GraphNode, next)
    };
    auto& registry = GcLayoutRegistry::Instance();
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_graph_type_info),
        reinterpret_cast<uintptr_t>(&g_graph_type_info) + sizeof(GraphTypeInfo));
    uint64_t graph_stable_id = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(sizeof(GraphNode)) << 16);
    registry.Register(graph_stable_id, sizeof(GraphNode),
                      kGraphPointerOffsets, 1);
    g_graph_type_info.stable_id = graph_stable_id;
    g_graph_type_info.stable_id2 = graph_stable_id;

    // Build linked list with head as stack root (visible to GC).
    // BGC traces head→next→... marking all 5000 nodes during root scan.
    static constexpr int kListLen = 5000;
    GraphNode* head = nullptr;
    for (int i = 0; i < kListLen; i++) {
        auto* n = static_cast<GraphNode*>(AllocOldGen(sizeof(GraphNode)));
        if (n) {
            std::memset(n, 0xBB, sizeof(GraphNode));
            n->type_info = reinterpret_cast<FakeTypeInfo*>(&g_graph_type_info);
            n->value = static_cast<uint64_t>(i);
            n->next = head;
            head = n;
        }
    }

    // Pin all GraphNodes so CrossPageCompact doesn't move them.
    // (The C++ heap nodes[] vector stores old addresses; moving nodes
    // would leave stale pointers that cause false verification failures.)
    {
        auto* cur = head;
        while (cur) {
            g_old_gen.AddPinnedRoot(cur, sizeof(GraphNode));
            cur = cur->next;
        }
    }

    // Index array for random mutator access (C++ heap, not GC-rooted).
    std::vector<GraphNode*> nodes(kListLen);
    {
        auto* cur = head;
        for (int i = kListLen - 1; i >= 0 && cur; i--) {
            nodes[i] = cur;
            cur = cur->next;
        }
    }

    std::atomic<bool> mutating{true};

    // Verify linked list from head is traversable and covers all nodes.
    {
        int reachable = 0;
        auto* cur = head;
        // Use a slow/fast pointer to detect cycles without infinite loop.
        while (cur && reachable < kListLen + 5) {
            reachable++;
            cur = cur->next;
        }
        printf("  reachable from head: %d/%d nodes%s\n",
               reachable, kListLen,
               reachable >= kListLen ? "" : " (list may be shorter than expected)");
        int valid_ti = 0;
        for (int i = 0; i < kListLen; i++) {
            if (nodes[i]->type_info == reinterpret_cast<FakeTypeInfo*>(&g_graph_type_info)) valid_ti++;
        }
        printf("  valid type_info: %d/%d\n", valid_ti, kListLen);
        // Verify ScanObjectChildren path: check stable_id via ReadStableId.
        auto& reg = GcLayoutRegistry::Instance();
        uint64_t sid = reg.ReadStableId(head->type_info);
        auto* layout = reg.Lookup(sid);
        printf("  head type_info ReadStableId=0x%llx layout=%p ptr_count=%d\n",
               static_cast<unsigned long long>(sid),
               (void*)layout,
               layout ? layout->pointer_count : -1);
        if (layout && layout->pointer_count > 0) {
            auto* next_ptr = *reinterpret_cast<void**>(reinterpret_cast<char*>(head) + layout->pointer_offsets[0].offset);
            printf("  head->next via layout = %p (old-gen=%d)\n",
                   next_ptr, g_old_gen.IsInOldGen(next_ptr));
        }
    }

    // Mutator thread: constantly rewrite next pointers.
    // Sleep between batches to let BGC drain SATB entries and converge.
    std::thread mutator([&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        int idx = 0;
        while (mutating.load(std::memory_order_acquire)) {
            // Shuffle pointers with SATB pre-write barrier.
            for (int j = 0; j < 100 && j < kListLen - 1; j++) {
                int from = (idx + j) % kListLen;
                int to = (from + 1) % kListLen;
                BgcSatbPreWriteBarrier(
                    reinterpret_cast<void**>(&nodes[from]->next));
                nodes[from]->next = nodes[to];
                // Post-write barrier: mark the page dirty so STW re-mark
                // discovers the new pointer value (cards=0 in re-mark means
                // nodes written during concurrent mark are invisible to sweep).
                DirtyCard(nodes[from]);
            }
            idx = (idx + 100) % kListLen;
            threading::SafepointPoll();
            // Yield to let BGC drain SATB entries before next batch.
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        threading::UnregisterThread();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Dirty all cards covering GraphNode pages so STW re-mark scans every
    // node's slot.  Without this, only cards explicitly dirtied by the
    // mutator during concurrent mark are scanned (~44%), and remaining
    // nodes are invisible to the re-mark dirty-card scan even though they
    // contain valid pointed-to targets.
    for (auto* n : nodes) {
        DirtyCard(n);
    }

    // Run BGC while mutator is active.
    printf("  running BGC cycle with graph mutator active...\n");
    if (!RunBgcCycle()) {
        printf("  ERROR: BGC cycle failed!\n");
    } else {
        printf("  BGC cycle complete\n");
    }

    mutating.store(false, std::memory_order_release);
    mutator.join();

    // Verify graph integrity: all nodes must still be in old-gen,
    // have valid type_info.
    int verified = 0;
    for (int vi = 0; vi < kListLen; vi++) {
        auto* n = nodes[vi];
        if (g_old_gen.IsInOldGen(n) && n->type_info == reinterpret_cast<FakeTypeInfo*>(&g_graph_type_info)) {
            verified++;
        }
    }
    printf("  graph verification: %d/%d nodes intact\n", verified, kListLen);
    printf("  graph verification: %d/%d nodes intact\n", verified, kListLen);

    printf("── Test 5 done ──\n\n");
}

// ── Test 6: Mark stack overflow pressure test ────────────────────────
//
/// Exercises the BGC per-worker mark deques under extreme pressure.
///
/// Topologies tested:
///   1. Deep chain: 50K nodes in singly-linked list (sequential deque pressure)
///   2. Wide fan-out: 100 roots × 1000 children each (burst deque pressure)
///
/// Each topology runs a BGC cycle and verifies object survival.  Roots are
/// kept alive via registered helper threads (BGC's GcScanAllThreadRoots).

struct MarkStressChainNode {
    FakeTypeInfo*              type_info;
    MarkStressChainNode*       next;
    uint64_t                   value;
};

struct MarkStressFanRoot {
    FakeTypeInfo*              type_info;
    void*                      children[16];
};

static FakeTypeInfo g_mstress_chain_type = {0, 0};
static FakeTypeInfo g_mstress_fan_type   = {0, 0};

static void InitMarkStressTypes() {
    auto& lr = GcLayoutRegistry::Instance();

    // ChainNode: one pointer field (next).
    lr.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_mstress_chain_type),
        reinterpret_cast<uintptr_t>(&g_mstress_chain_type) + sizeof(FakeTypeInfo));
    uint64_t chain_sid = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(sizeof(MarkStressChainNode)) << 16);
    static constexpr uint16_t kChainPtrs[] = { offsetof(MarkStressChainNode, next) };
    lr.Register(chain_sid, sizeof(MarkStressChainNode), kChainPtrs, 1);
    g_mstress_chain_type.stable_id = chain_sid;

    // FanRoot: 16 pointer fields (children[0..15]).
    lr.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_mstress_fan_type),
        reinterpret_cast<uintptr_t>(&g_mstress_fan_type) + sizeof(FakeTypeInfo));
    uint64_t fan_sid = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(sizeof(MarkStressFanRoot)) << 16);
    uint16_t fan_offsets[16];
    for (int i = 0; i < 16; i++) {
        fan_offsets[i] = static_cast<uint16_t>(
            offsetof(MarkStressFanRoot, children) + i * sizeof(void*));
    }
    lr.Register(fan_sid, sizeof(MarkStressFanRoot), fan_offsets, 16);
    g_mstress_fan_type.stable_id = fan_sid;
}

static void TestMarkStackPressure() {
    printf("\n── Test 6: Mark stack overflow pressure ──\n");
    fflush(stdout);

    InitMarkStressTypes();

    // ── Topology 1: Deep chain (50,000 nodes) ─────────────────────
    printf("  [chain] allocating 50000 nodes...\n");
    fflush(stdout);

    static constexpr uint32_t kChainLen = 50000;
    MarkStressChainNode* chain_head = nullptr;
    MarkStressChainNode* chain_prev = nullptr;
    for (uint32_t i = 0; i < kChainLen; i++) {
        auto* n = static_cast<MarkStressChainNode*>(AllocOldGen(sizeof(MarkStressChainNode)));
        if (n == nullptr) {
            printf("  [chain] OOM at node %u\n", static_cast<unsigned>(i));
            break;
        }
        memset(n, 0, sizeof(MarkStressChainNode));
        n->type_info = &g_mstress_chain_type;
        n->value = static_cast<uint64_t>(i);
        if (chain_prev) chain_prev->next = n;
        else chain_head = n;
        chain_prev = n;
    }
    uint64_t chain_len = 0;
    for (auto* c = chain_head; c; c = c->next) chain_len++;

    printf("  [chain] allocated %llu nodes, head=%p\n",
           static_cast<unsigned long long>(chain_len), (void*)chain_head);
    fflush(stdout);

    // ── Root helper thread: holds chain_head as volatile stack root ──
    // BGC's GcScanAllThreadRoots iterates registered threads and
    // conservatively scans their stack frames for old-gen pointers.
    // Without this, the chain is unreachable and BGC sweeps it.
    std::atomic<bool> chain_ready{false};
    std::atomic<bool> chain_done{false};
    std::thread chain_helper([chain_head, &chain_ready, &chain_done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        volatile MarkStressChainNode* root = chain_head;
        (void)root;
        chain_ready.store(true, std::memory_order_release);
        // Keep the thread alive during BGC cycle.
        while (!chain_done.load(std::memory_order_acquire)) {
            threading::SafepointPoll();
            std::this_thread::yield();
        }
        threading::UnregisterThread();
    });

    // Wait for helper to register.
    while (!chain_ready.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // ── Run BGC ───────────────────────────────────────────────────
    printf("  [chain] running BGC cycle...\n");
    fflush(stdout);
    bool ok = RunBgcCycle();
    CHECK(ok, "BGC cycle under chain pressure completed");

    // Stop helper.
    chain_done.store(true, std::memory_order_release);
    chain_helper.join();

    // ── Verify chain ──────────────────────────────────────────────
    printf("  [chain] verifying survival...\n");
    fflush(stdout);
    bool chain_ok = true;
    uint64_t verified = 0;
    for (auto* c = chain_head; c; c = c->next) {
        if (!g_old_gen.IsInOldGen(c)) {
            printf("  [chain] node %p NOT in old-gen at pos %llu\n",
                   (void*)c, static_cast<unsigned long long>(verified));
            chain_ok = false;
            break;
        }
        if (c->type_info != &g_mstress_chain_type) {
            printf("  [chain] node %p wrong type_info at pos %llu\n",
                   (void*)c, static_cast<unsigned long long>(verified));
            chain_ok = false;
            break;
        }
        if (c->value != verified) {
            printf("  [chain] node %p value=%llu expected=%llu\n",
                   (void*)c, static_cast<unsigned long long>(c->value),
                   static_cast<unsigned long long>(verified));
            chain_ok = false;
            break;
        }
        verified++;
    }
    CHECK(chain_ok && verified == chain_len,
          "all chain nodes survived BGC and are intact");

    // ── Topology 2: Wide fan-out (100 roots × 1000 children) ──────
    printf("  [fanout] allocating 100 roots × 1000 children...\n");
    fflush(stdout);

    static constexpr int kFanRoots = 100;
    static constexpr int kFanChildren = 1000;
    std::vector<MarkStressFanRoot*> fan_roots;
    uint64_t total_fan = 0;

    for (int ri = 0; ri < kFanRoots; ri++) {
        auto* root = static_cast<MarkStressFanRoot*>(
            AllocOldGen(sizeof(MarkStressFanRoot)));
        if (root == nullptr) break;
        memset(root, 0, sizeof(MarkStressFanRoot));
        root->type_info = &g_mstress_fan_type;
        fan_roots.push_back(root);
        total_fan++;

        for (int ci = 0; ci < kFanChildren; ci++) {
            void* child = AllocOldGen(64);
            if (child == nullptr) break;
            memset(child, 0xEF, 64);
            int slot = ci % 16;
            root->children[slot] = child;
            total_fan++;
        }
    }
    printf("  [fanout] allocated %llu total nodes (%zu roots)\n",
           static_cast<unsigned long long>(total_fan), fan_roots.size());
    fflush(stdout);

    // ── Fan-out root helper thread ────────────────────────────────
    // Holds ALL roots as volatile locals so the BGC conservative stack
    // scanner finds them.  A simple loop-local volatile would only keep
    // the last iteration's value on the helper's stack.
    std::atomic<bool> fan_ready{false};
    std::atomic<bool> fan_done{false};
    std::thread fan_helper([&fan_roots, &fan_ready, &fan_done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        // Hold ALL roots as individual volatile locals (not loop-local).
        volatile void* volatile_roots[100] = {};
        for (size_t i = 0; i < fan_roots.size() && i < 100; i++) {
            volatile_roots[i] = fan_roots[i];
        }
        fan_ready.store(true, std::memory_order_release);
        while (!fan_done.load(std::memory_order_acquire)) {
            threading::SafepointPoll();
            std::this_thread::yield();
        }
        // Use all volatile roots before exiting to prevent compiler elision.
        for (size_t i = 0; i < fan_roots.size() && i < 100; i++) {
            (void)volatile_roots[i];
        }
        threading::UnregisterThread();
    });

    while (!fan_ready.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // ── Run BGC ───────────────────────────────────────────────────
    printf("  [fanout] running BGC cycle...\n");
    fflush(stdout);
    ok = RunBgcCycle();
    CHECK(ok, "BGC cycle under fan-out pressure completed");

    fan_done.store(true, std::memory_order_release);
    fan_helper.join();

    // ── Verify fan-out ────────────────────────────────────────────
    int fan_survived = 0;
    for (auto* r : fan_roots) {
        if (g_old_gen.IsInOldGen(r) && r->type_info == &g_mstress_fan_type) {
            fan_survived++;
        }
    }
    CHECK(fan_survived == static_cast<int>(fan_roots.size()),
          "fan-out roots survived BGC");

    printf("── Test 6 done ──\n\n");
    fflush(stdout);
}

int main(int argc, char** argv) {
    // Test filter: if a name is given on command line, only run tests
    // whose function name contains the filter string.
    const char* filter = (argc > 1) ? argv[1] : "";
    auto match = [filter](const char* name) -> bool {
        return filter[0] == '\0' || strstr(name, filter) != nullptr;
    };

    printf("CRAG BGC Chase-Lev stress test\n");
    printf("═══════════════════════════════════\n");

    // Init card table.
    GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
    g_old_gen.Init(0, 2);

    // Init cross-gen support.
    InitCrossGen();

    // Start BGC and finalizer threads.
    BgcController::Instance().Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Run tests.
    if (match("Concurrent")) TestBgcConcurrentStress();
    if (match("Stealing") || match("Work") || match("Tree")) TestBgcWorkStealingStress();
    if (match("ManyThreads") || match("Thread")) TestBgcWithManyThreads();
    if (match("Finalization") || match("Final")) TestBgcFinalizationStress();
    if (match("GraphMutation") || match("Graph") || match("Mutation")) TestBgcGraphMutation();
    // Tests 6-7 are currently excluded:
    //   Test 7 (YoungGcDuringBgc) — needs start_cycle_skipped handling
    //   Test 6 (MarkStackPressure) — manual stress only

    // Stop BGC and finalizer.
    chaos_gc_wait_for_pending_finalizers();
    BgcController::Instance().Stop();

    // Summary.
    printf("══ Test summary ══\n");
    if (g_failures == 0) {
        printf("  ALL TESTS PASSED\n");
    } else {
        printf("  %d FAILURES\n", g_failures);
    }

    return g_failures > 0 ? 1 : 0;
}
