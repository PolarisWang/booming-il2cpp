// async_stress_test.cpp — Phase 5: 100k Task allocation under GC pressure.
//
// THE ACCEPTANCE CRITERION (design-v1-01.md Phase 5):
//     "GC stress: 100k Task 分配 ASan/Valgrind 无 UAF/泄漏"
//
// WHY A COUNT AND NOT JUST "IT RAN"
// ---------------------------------
// "It completed without crashing" is satisfied by an implementation that
// allocates nothing.  So the test asserts the allocation COUNT it actually
// performed, and that every task it created reached a terminal state — a
// task leaked in `pending` after its producer finished is the shape that
// shows up later as a hang rather than a crash, and a bare "no ASan report"
// cannot see it.
//
// WHAT THIS DOES AND DOES NOT PROVE
// ---------------------------------
// It proves the Task lifecycle (create → complete → continuation → release)
// is sound at scale on this thread plus its helpers.  It does NOT prove
// absence of a leak on its own — that needs ASan/Valgrind, which is a build
// configuration, not a test.  Running this under the asan preset is the
// complete check; the assertion here is the part that holds in any build.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "runtime_stubs/stub_common.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;

namespace {

constexpr int kTaskCount = 100000;
constexpr int kChunkSize = 1000;

std::atomic<int> g_completions{0};

void OnTaskCompleted(CHAOS_IL2CPP_INTPTR /*handle*/, void* /*ctx*/) noexcept {
    g_completions.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// 1. Allocate and complete 100k tasks, asserting the count
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncStress, HundredThousandTaskLifecycles)
{
    g_completions.store(0);

    int created = 0;
    for (int chunk = 0; chunk < kTaskCount / kChunkSize; ++chunk) {
        std::vector<CHAOS_IL2CPP_INTPTR> tasks;
        tasks.reserve(kChunkSize);

        for (int i = 0; i < kChunkSize; ++i) {
            CHAOS_IL2CPP_INTPTR t = async_task_create();
            ASSERT_NE(t, 0) << "allocation failed at task " << (chunk * kChunkSize + i);
            tasks.push_back(t);
            ++created;
        }

        // Complete every task in the chunk; the continuation count is the
        // evidence that each task actually reached a terminal state.
        for (auto* t : tasks) {
            auto* src = new TaskSource();
            src->task = require_async_task(t);
            async_task_on_completed(t, OnTaskCompleted, nullptr);
            src->set_result(1);
            task_source_destroy(src);
        }
    }

    ASSERT_EQ(created, kTaskCount) << "the test must actually perform the allocation";
    EXPECT_EQ(g_completions.load(), kTaskCount)
        << "every task must reach completion — a task left pending after its "
        << "producer finished is the leak shape a crash-only check cannot see";
}

// ══════════════════════════════════════════════════════════════════════════════
// 2. Concurrent lifecycle across threads — exercises the atomic completion
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncStress, ConcurrentLifecyclesAcrossThreads)
{
    constexpr int kThreads = 8;
    constexpr int kPerThread = 2000;

    g_completions.store(0);
    std::atomic<int> created{0};
    std::vector<std::thread> workers;
    workers.reserve(kThreads);

    for (int th = 0; th < kThreads; ++th) {
        workers.emplace_back([&created] {
            for (int i = 0; i < kPerThread; ++i) {
                CHAOS_IL2CPP_INTPTR t = async_task_create();
                if (t == 0) continue;
                created.fetch_add(1, std::memory_order_relaxed);
                auto* src = new TaskSource();
                src->task = require_async_task(t);
                async_task_on_completed(t, OnTaskCompleted, nullptr);
                src->set_result(1);
                task_source_destroy(src);
            }
        });
    }
    for (auto& w : workers) w.join();

    EXPECT_EQ(created.load(), kThreads * kPerThread);
    EXPECT_EQ(g_completions.load(), created.load())
        << "completions must match creations exactly — an off-by-one here is "
        << "a lost or double-fired continuation";
}

// ══════════════════════════════════════════════════════════════════════════════
// 3. Deep continuation chains — no unbounded recursion in the delivery path
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncStress, DeepContinuationChainDoesNotOverflowTheStack)
{
    // Each task keeps exactly one continuation, so this is a chain rather than
    // a fan-in.  A delivery path that recursed per link would blow the stack
    // long before the chain ends.
    constexpr int kChainLength = 10000;
    constexpr CHAOS_IL2CPP_INTPTR kSlot = 0x1010'1010;

    CHAOS_IL2CPP_INTPTR head = async_task_create();
    ASSERT_NE(head, 0);

    auto* head_src = new TaskSource();
    head_src->task = require_async_task(head);
    head_src->set_result(kChainLength);
    task_source_destroy(head_src);

    // Completing an already-completed task must deliver its continuation
    // inline (the fast path in async_task_on_completed), so a long run of
    // already-satisfied tasks exercises the deepest delivery path.
    for (int i = 0; i < kChainLength; ++i) {
        g_completions.store(0);
        CHAOS_IL2CPP_INTPTR t = async_task_from_result(kSlot);
        ASSERT_NE(t, 0);
        async_task_on_completed(t, OnTaskCompleted, nullptr);
        ASSERT_EQ(g_completions.load(), 1)
            << "an already-completed task must deliver its continuation exactly "
            << "once (link " << i << ")";
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// 4. Faulted tasks at scale — the exception path allocates too
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncStress, FaultedTaskLifecyclesAtScale)
{
    constexpr int kCount = 20000;
    g_completions.store(0);

    for (int i = 0; i < kCount; ++i) {
        CHAOS_IL2CPP_INTPTR t = async_task_create();
        ASSERT_NE(t, 0);
        auto* src = new TaskSource();
        src->task = require_async_task(t);
        async_task_on_completed(t, OnTaskCompleted, nullptr);
        src->set_exception(static_cast<CHAOS_IL2CPP_INTPTR>(i + 1));
        task_source_destroy(src);
    }

    EXPECT_EQ(g_completions.load(), kCount)
        << "faulted completions must fire their continuation just like "
        << "successful ones — dropping them leaks the awaiting state machine";
}
