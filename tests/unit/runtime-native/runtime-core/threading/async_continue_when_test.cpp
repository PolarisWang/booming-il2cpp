// async_continue_when_test.cpp — TaskFactory.ContinueWhenAll / ContinueWhenAny.
//
// These compose two existing primitives: the WhenAll/WhenAny aggregate and the
// ContinueWith delivery path.  The composition has exactly one way to be wrong
// that still looks right, and it is the reason this file exists.
//
// THE COUNTEREXAMPLE
// ------------------
// Registering the continuation on each CHILD instead of on the AGGREGATE fires
// it once per task.  For ContinueWhenAll that is N invocations where the caller
// asked for one — side effects multiply, and the bug is invisible on a
// single-element array, which is exactly what a naive test would use.
//
// The central test therefore uses THREE tasks and asserts the continuation ran
// EXACTLY ONCE.  A per-child implementation passes any "the continuation ran"
// assertion and fails this one.
//
// The second property is which object the continuation observes.  For
// ContinueWhenAll the continuation must receive the AGGREGATE (whose result is
// the per-index value array), not whichever child happened to finish last.
// Reusing the child's handle would make `t => t.Result` read a scalar where the
// caller expects the aggregate.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/async.h>

#include "async_stubs.h"
#include "core/delegate_object.h"
#include "runtime_stubs/stub_common.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;
using chaos::il2cpp::runtime_core::DelegateObject;

namespace {

std::atomic<int>                 g_runs{0};
std::atomic<CHAOS_IL2CPP_INTPTR> g_seen_antecedent{0};
std::atomic<CHAOS_IL2CPP_INTPTR> g_seen_result{0};

/// Continuation body: records how many times it ran and what it observed, then
/// resolves its own task with the observed result so the caller can chain.
CHAOS_IL2CPP_INTPTR ContinuationBody(CHAOS_IL2CPP_INTPTR arg) {
    g_seen_antecedent.store(arg);
    auto* antecedent = reinterpret_cast<AsyncTask*>(arg);
    if (antecedent != nullptr) g_seen_result.store(antecedent->result);
    g_runs.fetch_add(1);
    return g_seen_result.load();
}

DelegateObject* MakeDelegate(CHAOS_IL2CPP_INTPTR (*fn)(CHAOS_IL2CPP_INTPTR)) {
    auto* del = new DelegateObject();
    del->type_info = nullptr;
    del->chaos_delegate_target = 0;
    del->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fn);
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    del->_pad = 0;
    return del;
}

void ResetObservations() {
    g_runs.store(0);
    g_seen_antecedent.store(0);
    g_seen_result.store(0);
}

constexpr auto kPollInterval = std::chrono::milliseconds(2);
constexpr int  kMaxPolls = 1500;

bool WaitFor(const std::function<bool()>& pred) {
    for (int i = 0; i < kMaxPolls; ++i) {
        if (pred()) return true;
        std::this_thread::sleep_for(kPollInterval);
    }
    return pred();
}

CHAOS_IL2CPP_INTPTR MakeTaskArray(const std::vector<CHAOS_IL2CPP_INTPTR>& handles) {
    const size_t n = handles.size();
    const size_t bytes = sizeof(ManagedArrayAccessor) + n * sizeof(CHAOS_IL2CPP_INTPTR);
    auto* raw = static_cast<uint8_t*>(std::malloc(bytes));
    std::memset(raw, 0, bytes);
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(raw);
    arr->element_type_shape = 0;
    arr->element_type_info = nullptr;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(n);
    auto* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(raw + sizeof(ManagedArrayAccessor));
    for (size_t i = 0; i < n; ++i) elements[i] = handles[i];
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

void FreeTaskArray(CHAOS_IL2CPP_INTPTR handle) {
    std::free(reinterpret_cast<void*>(handle));
}

CHAOS_IL2CPP_INTPTR Resolved(CHAOS_IL2CPP_INTPTR value) {
    return async_task_from_result(value);
}

CHAOS_IL2CPP_INTPTR Pending() { return async_task_create(); }

TaskSource* SourceFor(CHAOS_IL2CPP_INTPTR handle) {
    auto* src = new TaskSource();
    src->task = require_async_task(handle);
    return src;
}

void CompleteLater(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value,
                   std::chrono::milliseconds delay) {
    std::thread([handle, value, delay] {
        std::this_thread::sleep_for(delay);
        auto* src = SourceFor(handle);
        src->set_result(value);
        task_source_destroy(src);
    }).detach();
}

}  // namespace

// ══════════════════════════════════════════════════════════════════════════════
// ContinueWhenAll — runs ONCE, after every task, on the aggregate
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWhen, AllFiresExactlyOnceAfterEveryChildCompletes)
{
    ResetObservations();

    CHAOS_IL2CPP_INTPTR a = Pending();
    CHAOS_IL2CPP_INTPTR b = Pending();
    CHAOS_IL2CPP_INTPTR c = Pending();

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, b, c});
    DelegateObject* cont = MakeDelegate(ContinuationBody);
    CHAOS_IL2CPP_INTPTR result =
        chaos_task_continue_when_all_array(arr, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(cont));
    ASSERT_NE(result, 0);
    FreeTaskArray(arr);

    // Stagger the completions so the last one is unambiguous.
    CompleteLater(a, 10, std::chrono::milliseconds(5));
    CompleteLater(b, 20, std::chrono::milliseconds(20));
    CompleteLater(c, 30, std::chrono::milliseconds(45));

    ASSERT_TRUE(WaitFor([] { return g_runs.load() >= 1; }))
        << "the continuation never ran";

    // THE load-bearing assertion: exactly once, not once per child.  A
    // continuation registered on each child would already have run 3 times.
    // Give any late per-child firings a chance to land, then check.
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    EXPECT_EQ(g_runs.load(), 1)
        << "ContinueWhenAll must fire ONCE after all tasks, not once per task";

    // It observed the AGGREGATE, not an individual child.
    CHAOS_IL2CPP_INTPTR seen = g_seen_antecedent.load();
    EXPECT_NE(seen, a) << "continuation should receive the aggregate, not a child";
    EXPECT_NE(seen, b);
    EXPECT_NE(seen, c);
    EXPECT_NE(seen, 0);

    delete cont;
}

// ══════════════════════════════════════════════════════════════════════════════
// ContinueWhenAny — runs once, on the first completion
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWhen, AnyFiresExactlyOnceOnFirstCompletion)
{
    ResetObservations();

    CHAOS_IL2CPP_INTPTR a = Pending();
    CHAOS_IL2CPP_INTPTR b = Pending();

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, b});
    DelegateObject* cont = MakeDelegate(ContinuationBody);
    CHAOS_IL2CPP_INTPTR result =
        chaos_task_continue_when_any_array(arr, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(cont));
    ASSERT_NE(result, 0);
    FreeTaskArray(arr);

    CompleteLater(a, 10, std::chrono::milliseconds(30));
    CompleteLater(b, 20, std::chrono::milliseconds(5));  // b wins

    ASSERT_TRUE(WaitFor([] { return g_runs.load() >= 1; }))
        << "the continuation never ran on the first completion";

    // Even after the LOSING task completes, the continuation must not run again.
    ASSERT_TRUE(WaitFor([&] {
        return require_async_task(a)->completed.load(std::memory_order_acquire) != 0;
    })) << "the losing task never completed";
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    EXPECT_EQ(g_runs.load(), 1)
        << "ContinueWhenAny must fire once; a second firing means it watched a child";

    delete cont;
}

// ══════════════════════════════════════════════════════════════════════════════
// Already-completed array — the common generated-code case
// ══════════════════════════════════════════════════════════════════════════════

TEST(AsyncContinueWhen, AllFiresWhenEveryChildAlreadyCompleted)
{
    ResetObservations();

    CHAOS_IL2CPP_INTPTR a = Resolved(1);
    CHAOS_IL2CPP_INTPTR b = Resolved(2);

    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a, b});
    DelegateObject* cont = MakeDelegate(ContinuationBody);
    CHAOS_IL2CPP_INTPTR result =
        chaos_task_continue_when_all_array(arr, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(cont));
    ASSERT_NE(result, 0);
    FreeTaskArray(arr);

    ASSERT_TRUE(WaitFor([] { return g_runs.load() >= 1; }))
        << "an already-satisfied ContinueWhenAll must still run its continuation";
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    EXPECT_EQ(g_runs.load(), 1);

    delete cont;
}

TEST(AsyncContinueWhen, InvalidContinuationIsRejected)
{
    CHAOS_IL2CPP_INTPTR a = Resolved(1);
    CHAOS_IL2CPP_INTPTR arr = MakeTaskArray({a});

    EXPECT_EQ(chaos_task_continue_when_all_array(arr, 0), 0);
    EXPECT_EQ(chaos_task_continue_when_any_array(arr, 0), 0);

    FreeTaskArray(arr);
}
