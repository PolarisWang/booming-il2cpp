// synchronization_context.cpp — SynchronizationContext placeholder + dispatch.
//
// See synchronization_context.h for design rationale.
//
// THE REAL IMPLEMENTATION
// -----------------------
// A placeholder SynchronizationContext (the kind `new SynchronizationContext()`
// creates) has no UI pump to marshal to, so it uses the ThreadPool as its
// transport:
//
//   Post -> queue on the ThreadPool   (async, returns before the callback runs)
//   Send -> invoke inline             (sync, no pump to marshal to)
//
// `Send` being inline is the honest choice: the alternative is blocking the
// calling thread until a pump that does not exist drains the callback.  `Post`
// is genuinely asynchronous — a real UI-thread context installed by
// `SetSynchronizationContext` would override it to queue to its own pump; the
// placeholder queues to the pool instead of silently running inline.
//
// The extern "C" bridge at the bottom is what generated C++ calls via the
// ShapeRegistry; the namespace members above it are the runtime-internal API.

#include "synchronization_context.h"
#include "thread_pool.h"

#include <cstdlib>
#include <new>
#include <thread>

namespace chaos::il2cpp::runtime_core::threading {

namespace {

// thread_local for the current thread's SynchronizationContext.
thread_local SynchronizationContext* tls_current_ctx = nullptr;

/// Trampoline payload for an asynchronous Post.  The ThreadPool work-item ABI is
/// a bare `void(*)(void*)`, and Post needs both the callback and its state, so
/// they travel together in one heap node that the worker frees after running.
struct PostWork {
    void (*callback)(void*);
    void* state;

    static void Run(void* self) noexcept {
        auto* work = static_cast<PostWork*>(self);
        void (*cb)(void*) = work->callback;
        void* st = work->state;
        delete work;
        cb(st);
    }
};

}  // anonymous namespace

SynchronizationContext* SynchronizationContextCreate() noexcept
{
    auto* ctx = new (std::nothrow) SynchronizationContext();
    return ctx;  // nullptr on allocation failure is the correct signal
}

void SynchronizationContextDestroy(SynchronizationContext* ctx) noexcept
{
    if (ctx == nullptr) return;
    if (tls_current_ctx == ctx)
        tls_current_ctx = nullptr;
    delete ctx;
}

SynchronizationContext* SynchronizationContextSetCurrent(SynchronizationContext* ctx) noexcept
{
    auto* prev = tls_current_ctx;
    tls_current_ctx = ctx;
    return prev;
}

SynchronizationContext* SynchronizationContextGetCurrent() noexcept
{
    return tls_current_ctx;
}

bool SynchronizationContextPost(SynchronizationContext* ctx,
                                void (*callback)(void*), void* state) noexcept
{
    // Asynchronous by contract (see the header): queue the callback on the
    // ThreadPool and return WITHOUT running it.  The caller must not be blocked
    // and must not observe the callback run before Post returns.
    //
    // This used to invoke `callback(state)` inline, which made Post and Send
    // behaviourally identical.  That is not a harmless simplification: managed
    // code distinguishes the two — ConfigureAwait(true) continuation ordering,
    // deadlock-avoidance patterns, and any "did this actually run later?" check
    // all rely on Post returning first.  An inline Post also re-enters the
    // caller's stack, so a callback that takes a lock already held by the
    // caller self-deadlocks where real Post would not.
    //
    // The queue boundary needs to own the callback/state pair until a worker
    // picks it up, so stash them together rather than passing `state` through
    // as the context (which would lose the callback).
    if (ctx == nullptr || callback == nullptr) return false;

    auto* work = new (std::nothrow) PostWork{callback, state};
    if (work == nullptr) return false;

    // ExecutionContext capture (the safe entry point): a posted callback that
    // touches AsyncLocal/ExecutionContext must see the ambient state at Post
    // time, not whatever the picking worker happens to have.
    ThreadPoolQueueUserWorkItem(&PostWork::Run, work);
    return true;
}

bool SynchronizationContextSend(SynchronizationContext* ctx,
                                void (*callback)(void*), void* state) noexcept
{
    if (ctx == nullptr || callback == nullptr) return false;
    callback(state);
    return true;
}

}  // namespace chaos::il2cpp::runtime_core::threading

// ══════════════════════════════════════════════════════════════════════════════
// extern "C" bridges for codegen-emitted C++ (Phase 3 — SynchronizationContext)
// ══════════════════════════════════════════════════════════════════════════════

extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_create() noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        chaos::il2cpp::runtime_core::threading::SynchronizationContextCreate());
}

extern "C" void chaos_synchronization_context_destroy(CHAOS_IL2CPP_INTPTR ctx) noexcept
{
    chaos::il2cpp::runtime_core::threading::SynchronizationContextDestroy(
        reinterpret_cast<chaos::il2cpp::runtime_core::threading::SynchronizationContext*>(ctx));
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_get_current() noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        chaos::il2cpp::runtime_core::threading::SynchronizationContextGetCurrent());
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_set_current(
    CHAOS_IL2CPP_INTPTR ctx) noexcept
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        chaos::il2cpp::runtime_core::threading::SynchronizationContextSetCurrent(
            reinterpret_cast<chaos::il2cpp::runtime_core::threading::SynchronizationContext*>(ctx)));
}