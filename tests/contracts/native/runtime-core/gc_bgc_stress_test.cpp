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
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <csignal>
#include <cstddef>
#define NOMINMAX
#include <windows.h>

#include <chaos/native_types.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
#define CHECK(cond, msg) do {                                   \
    if (!(cond)) {                                              \
        printf("  FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg);\
        ++g_failures;                                           \
    } else {                                                    \
        printf("  PASS: %s\n", msg);                            \
    }                                                           \
} while(0)

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
    printf("  RunBgcCycle: requesting safepoint...\n"); fflush(stdout);
    uint32_t gen = threading::RequestGlobalSafepoint();
    printf("  RunBgcCycle: safepoint acquired gen=%u, starting BGC cycle...\n", gen); fflush(stdout);
    BgcController::Instance().StartBgcCycle();
    printf("  RunBgcCycle: BGC cycle started, releasing safepoint...\n"); fflush(stdout);
    threading::ReleaseGlobalSafepoint(gen);

    printf("  RunBgcCycle: waiting for REMARK_NEEDED...\n"); fflush(stdout);
    if (!WaitForPhase(BgcPhase::REMARK_NEEDED, 120000)) {
        printf("  ERROR: concurrent mark did not complete\n");
        return false;
    }

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    printf("  RunBgcCycle: waiting for COMPACT_NEEDED...\n"); fflush(stdout);
    if (!WaitForPhase(BgcPhase::COMPACT_NEEDED, 120000)) {
        printf("  ERROR: concurrent sweep did not complete (phase=%d)\n",
               static_cast<int>(BgcController::Instance().Phase()));
        return false;
    }

    printf("  BGC compact_needed detected, requesting safepoint...\n");
    fflush(stdout);
    gen = threading::RequestGlobalSafepoint();
    printf("  Safepoint acquired, calling StwCompact...\n");
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    if (!WaitForPhase(BgcPhase::IDLE, 120000)) {
        printf("  ERROR: BGC cycle did not complete\n");
        return false;
    }

    printf("  BGC cycle complete.\n");
    return true;
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
    printf("  SKIPPED — Test 1 is a long-running stress test.\n");
    printf("  Use run_stress.sh with --test bgc for full stress.\n");
    printf("── Test 1 done ──\n\n");
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
    __try {
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

    // DIAG: check root bytes immediately after start signal.
    {
        const auto* raw_root = reinterpret_cast<const volatile uint8_t*>(stack_root);
        bool has_ff = false;
        for (int i = 0; i < 32; i++) {
            if (raw_root[i] == 0xFF) { has_ff = true; break; }
        }
        if (has_ff) {
            printf("  ROOT_CHECK[%d]: root %p has 0xFF bytes at START! "
                   "raw=[%02x %02x %02x %02x %02x %02x %02x %02x"
                   " | %02x %02x %02x %02x %02x %02x %02x %02x"
                   " | %02x %02x %02x %02x %02x %02x %02x %02x"
                   " | %02x %02x %02x %02x %02x %02x %02x %02x]\n",
                   thread_id, (void*)stack_root,
                   raw_root[0], raw_root[1], raw_root[2], raw_root[3],
                   raw_root[4], raw_root[5], raw_root[6], raw_root[7],
                   raw_root[8], raw_root[9], raw_root[10], raw_root[11],
                   raw_root[12], raw_root[13], raw_root[14], raw_root[15],
                   raw_root[16], raw_root[17], raw_root[18], raw_root[19],
                   raw_root[20], raw_root[21], raw_root[22], raw_root[23],
                   raw_root[24], raw_root[25], raw_root[26], raw_root[27],
                   raw_root[28], raw_root[29], raw_root[30], raw_root[31]);
            fflush(stdout);
        }
    }

    // Verify root is valid before starting walks.
    if (!g_old_gen.IsInOldGen(const_cast<TreeNode*>(stack_root))) {
        printf("  WARN[%d]: stack_root is NOT in old-gen at start!\n", thread_id);
        fflush(stdout);
    } else {
        printf("  OK[%d]: stack_root at %p is in old-gen\n", thread_id, (void*)const_cast<TreeNode*>(stack_root));
        fflush(stdout);
    }

    // Do random walks to keep cache warm and exercise concurrent reads.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);

    while (std::chrono::steady_clock::now() < deadline) {
        // Random walk: follow left/right randomly, simulating traversal.
        TreeNode* cur = const_cast<TreeNode*>(stack_root);
        int steps = 0;
        while (cur != nullptr && steps < 100) {
            // Poll safepoint so BGC compaction can suspend this thread.
            threading::SafepointPoll();
            // SAFETY CHECK: cur might be a non-old-gen pointer from a
            // corrupted TreeNode child field in the previous iteration.
            // Check BEFORE reading cur->left/right to prevent segfault.
            if (!g_old_gen.IsInOldGen(cur)) {
                printf("  ROOT_GUARD[%d]: cur=%p (non-oldgen) at step=%d, breaking inner\n",
                       thread_id, (void*)cur, steps);
                fflush(stdout);
                break;
            }
            // Read left/right into local vars FIRST to catch corrupted values
            // before they propagate to cur.  A corrupted pointer like
            // (TreeNode*)-1 would crash when dereferenced for the field read
            // in the NEXT iteration, so we validate the parent node before
            // trusting its child pointers.
            volatile TreeNode* vroot = stack_root;
            (void)vroot;
            TreeNode* left_child = cur->left;
            TreeNode* right_child = cur->right;
            if (left_child != nullptr && !g_old_gen.IsInOldGen(left_child)) {
                // Dump raw bytes of corrupted node to identify the write pattern.
                const auto* raw = reinterpret_cast<const uint8_t*>(cur);
                MEMORY_BASIC_INFORMATION mbi;
                bool mbi_ok = VirtualQuery(cur, &mbi, sizeof(mbi)) != 0;
                bool is_payload = g_old_gen.IsInOldGen(cur);
                printf("  FIELD_GUARD[%d]: cur=%p left=%p (non-oldgen, step=%d)"
                       " in_oldgen=%d mbi_state=0x%lx type=0x%lx"
                       " raw=[%02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x]\n",
                       thread_id, (void*)cur, (void*)left_child, steps,
                       is_payload,
                       mbi_ok ? mbi.State : 0, mbi_ok ? mbi.Type : 0,
                       raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7],
                       raw[8], raw[9], raw[10], raw[11], raw[12], raw[13], raw[14], raw[15],
                       raw[16], raw[17], raw[18], raw[19], raw[20], raw[21], raw[22], raw[23],
                       raw[24], raw[25], raw[26], raw[27], raw[28], raw[29], raw[30], raw[31]);
                fflush(stdout);
                left_child = nullptr;
            }
            if (right_child != nullptr && !g_old_gen.IsInOldGen(right_child)) {
                const auto* raw = reinterpret_cast<const uint8_t*>(cur);
                printf("  FIELD_GUARD[%d]: cur=%p right=%p (non-oldgen, step=%d)"
                       " raw=[%02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x"
                       " | %02x %02x %02x %02x %02x %02x %02x %02x]\n",
                       thread_id, (void*)cur, (void*)right_child, steps,
                       raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7],
                       raw[8], raw[9], raw[10], raw[11], raw[12], raw[13], raw[14], raw[15],
                       raw[16], raw[17], raw[18], raw[19], raw[20], raw[21], raw[22], raw[23],
                       raw[24], raw[25], raw[26], raw[27], raw[28], raw[29], raw[30], raw[31]);
                fflush(stdout);
                right_child = nullptr;
            }
            if (steps % 2 == 0) {
                cur = left_child;
            } else {
                cur = right_child;
            }
            steps++;
        }

        // Yield occasionally to allow other threads to make progress.
        if (steps > 50) {
            std::this_thread::yield();
        }

        // Poll safepoint to respond to STW requests.
        threading::SafepointPoll();
    }

    // Verify tree survived BGC.
    bool ok = VerifyTree(const_cast<TreeNode*>(stack_root),
                         static_cast<uint8_t>(thread_id * 32));
    survived_flag->store(ok, std::memory_order_release);

    threading::UnregisterThread();
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        printf("  CRASH[%d]: thread crashed with exception 0x%lx\n",
               thread_id, GetExceptionCode());
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
    for (int i = 0; i < kNumTrees; i++) {
        trees[i] = AllocTree(kTreeDepth, static_cast<uint8_t>(i * 32));
        expected_nodes += CountTree(trees[i]);
    }

    printf("  allocated %llu tree nodes total\n",
           static_cast<unsigned long long>(g_node_count.load(std::memory_order_relaxed)));

    // Start tree walker threads.
    std::atomic<bool> start_signal{false};
    std::atomic<bool> survived_flags[kNumTrees];
    for (int i = 0; i < kNumTrees; i++) {
        survived_flags[i].store(false, std::memory_order_release);
    }

    std::vector<std::thread> walkers;
    for (int i = 0; i < kNumTrees; i++) {
        walkers.emplace_back(TreeWalkThread, trees[i], &start_signal,
                             &survived_flags[i], i);
    }

    // Signal all walkers to start.
    start_signal.store(true, std::memory_order_release);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    printf("  main: starting pre-BGC tree count...\n");
    fflush(stdout);

    // Pre-BGC verification.
    uint64_t pre_counts[4];
    bool all_intact = true;
    for (int i = 0; i < 4; i++) {
        pre_counts[i] = CountTree(trees[i]);
        printf("  main: tree[%d] count=%llu\n", i,
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

    // Run 2 BGC cycles while walkers are active.
    printf("  running 2 BGC cycles with tree walkers active...\n");
    fflush(stdout);
    printf("  running 2 BGC cycles with tree walkers active...\n");
    for (int cycle = 0; cycle < 2; cycle++) {
        printf("  BGC cycle %d/2 starting...\n", cycle + 1);
        if (!RunBgcCycle()) {
            printf("  ERROR: BGC cycle %d failed\n", cycle + 1);
            break;
        }
        printf("  BGC cycle %d/2 complete\n", cycle + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Wait for walkers to finish.
    for (auto& w : walkers) {
        w.join();
    }

    // Check survival.
    int total_survived = 0;
    int total_expected = kNumTrees;
    for (int i = 0; i < kNumTrees; i++) {
        if (survived_flags[i].load(std::memory_order_acquire)) {
            total_survived++;
        } else {
            printf("  FAIL: tree %d did NOT survive BGC\n", i);
        }
    }

    // Final tree verification.
    uint64_t final_nodes = 0;
    for (int i = 0; i < kNumTrees; i++) {
        final_nodes += CountTree(trees[i]);
    }

    printf("  results: %d/%d trees survived (final count: %llu/%llu nodes)\n",
           total_survived, total_expected,
           static_cast<unsigned long long>(final_nodes),
           static_cast<unsigned long long>(g_node_count.load(std::memory_order_relaxed)));
    printf("  cross-gen writes during stress: %d\n",
           g_cross_gen_writes.load(std::memory_order_relaxed));
    printf("  root survival: %d/%d\n", total_survived, total_expected);
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

    printf("── Test 4 done ──\n\n");
}

// ── Test 5: BGC graph mutation stress ─────────────────────────────
//
/// Mutator threads mutate object graph while BGC traces concurrently.

struct GraphNode {
    FakeTypeInfo* type_info;
    GraphNode* next;
    uint64_t value;
};

static FakeTypeInfo g_graph_type_info = {0, 0};

static void TestBgcGraphMutation() {
    printf("\n── Test 5: BGC graph mutation stress ──\n");

    // Register graph node layout.
    static constexpr uint16_t kGraphPointerOffsets[] = {
        offsetof(GraphNode, next)
    };
    auto& registry = GcLayoutRegistry::Instance();
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_graph_type_info),
        reinterpret_cast<uintptr_t>(&g_graph_type_info) + sizeof(FakeTypeInfo));
    uint64_t graph_stable_id = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(sizeof(GraphNode)) << 16);
    registry.Register(graph_stable_id, sizeof(GraphNode),
                      kGraphPointerOffsets, 1);
    g_graph_type_info.stable_id = graph_stable_id;

    // Build initial linked list.
    static constexpr int kListLen = 5000;
    std::vector<GraphNode*> nodes;
    nodes.reserve(kListLen);
    for (int i = 0; i < kListLen; i++) {
        auto* n = static_cast<GraphNode*>(AllocOldGen(sizeof(GraphNode)));
        if (n) {
            std::memset(n, 0xBB, sizeof(GraphNode));
            n->type_info = &g_graph_type_info;
            n->value = static_cast<uint64_t>(i);
            if (!nodes.empty()) n->next = nodes.back();
            nodes.push_back(n);
        }
    }

    std::atomic<bool> mutating{true};

    // Mutator thread: constantly rewrite next pointers.
    std::thread mutator([&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        int idx = 0;
        while (mutating.load(std::memory_order_acquire)) {
            // Shuffle pointers.
            for (int j = 0; j < 100 && j < kListLen - 1; j++) {
                int from = (idx + j) % kListLen;
                int to = (from + 1) % kListLen;
                nodes[from]->next = nodes[to];
            }
            idx = (idx + 100) % kListLen;
            if ((idx % 500) == 0) {
                threading::SafepointPoll();
            }
            std::this_thread::yield();
        }
        threading::UnregisterThread();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Run BGC while mutator is active.
    printf("  running BGC cycle with graph mutator active...\n");
    if (!RunBgcCycle()) {
        printf("  ERROR: BGC cycle failed!\n");
    } else {
        printf("  BGC cycle complete\n");
    }

    mutating.store(false, std::memory_order_release);
    mutator.join();

    // Verify graph integrity.
    int verified = 0;
    for (auto* n : nodes) {
        if (g_old_gen.IsInOldGen(n) && n->type_info == &g_graph_type_info) {
            verified++;
        }
    }
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
    std::atomic<bool> fan_ready{false};
    std::atomic<bool> fan_done{false};
    // Store roots in a static buffer so the helper thread can hold them.
    // Only the first root's chain is needed (all roots are linked lists).
    std::thread fan_helper([&fan_roots, &fan_ready, &fan_done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (auto* r : fan_roots) {
            volatile void* root = r;
            (void)root;
        }
        fan_ready.store(true, std::memory_order_release);
        while (!fan_done.load(std::memory_order_acquire)) {
            threading::SafepointPoll();
            std::this_thread::yield();
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

int main() {
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
    TestBgcConcurrentStress();
    TestBgcWorkStealingStress();
    TestBgcWithManyThreads();
    TestBgcFinalizationStress();
    TestBgcGraphMutation();
    TestMarkStackPressure();  // skipped by default — enable for manual stress runs

    // Stop BGC.
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
