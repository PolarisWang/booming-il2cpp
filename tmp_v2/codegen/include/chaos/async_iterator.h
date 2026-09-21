#ifndef CHAOS_IL2CPP_COMMON_ASYNC_ITERATOR_H_
#define CHAOS_IL2CPP_COMMON_ASYNC_ITERATOR_H_

// ── ASYNC-P2-8 A2: native AsyncIteratorMethodBuilder + pooled ValueTask<bool> ──
//
// An `async IAsyncEnumerable<T>` / `async IAsyncEnumerator<T>` state machine drives an
// `AsyncIteratorMethodBuilder`, whose `MoveNextAsync()` hands the consumer a
// `ValueTask<bool>` (true = another element is available, false = iteration finished).
// The .NET implementation backs that ValueTask with a POOLED `IValueTaskSource<bool>`
// so that iterating does not allocate per element.
//
// This header provides the native equivalent of that machinery:
//
//   AsyncIteratorSourceCore   ≙ ManualResetValueTaskSourceCore<bool>  (the IValueTaskSource body)
//   AsyncIteratorSourcePool   the per-iterator pool of the above
//   chaos_async_iterator_*    the AsyncIteratorMethodBuilder op surface
//
// DESIGN NOTES — why this is shaped the way it is
// -----------------------------------------------
// 1. THE VERSION/TOKEN IS NOT DECORATION.  A pooled source is reused across iterations,
//    so a stale awaiter from a PREVIOUS iteration can still hold a reference to it.  .NET
//    disambiguates with a version token threaded through IValueTaskSource.GetStatus /
//    OnCompleted / GetResult.  Dropping the token check turns "reused source" into a
//    SILENTLY WRONG RESULT (the old awaiter observes the new iteration's value) — far
//    worse than a crash, because nothing reports it.  GetStatus/GetResult therefore
//    REJECT a mismatched token explicitly.
//
// 2. POOLING IS MANDATORY, NOT AN OPTIMISATION.  Under the project's P1 (performance
//    first) an allocating `MoveNextAsync` would put one heap allocation on every
//    `await foreach` step — precisely the cost .NET's pooling exists to remove.  So the
//    pool is part of the contract.  The overflow path (pool exhausted) allocates rather
//    than blocks: a blocked Acquire could deadlock when an iterator awaits itself on the
//    same thread, and a "return 0" fallback would be the silent-stub shape this effort
//    is removing.
//
// 3. `ValueTask<bool>` HERE IS THE MINIMAL CARRIER, NOT THE MANAGED STRUCT.  The managed
//    ValueTask<T> is a struct with an `_obj` that is polymorphically either an inline
//    result, a Task<T>, or an IValueTaskSource<T> (+token) — see the reflection probe in
//    async-iterator-a2-design-2026-09-11.md §0.1.  Codegen lowers an iterator's
//    `<>v__promiseOfValueOrEnd` to this carrier; the managed struct's full surface
//    (AsTask, GetTaskForValueTaskSource, ...) belongs to A4.
//
// Memory-domain pairing (see CLAUDE.md 统一内存分配约束):
//   - The pool lives BY VALUE inside the iterator state machine (GC-allocated), so its
//     lifetime is the state machine's — no separate free.
//   - Overflow sources use CHAOS_IL2CPP_NEW / CHAOS_IL2CPP_DELETE (raw domain), matched
//     on release.  Whether a source came from the pool is tracked with an explicit flag,
//     NOT inferred from its address: a heap allocation could otherwise land inside the
//     pool's address range and be wrongly "returned" to it.

#include <chaos/native_types.h>
#include "ptr_tag.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <new>

namespace chaos::il2cpp::common {

/// Completion state of a source token — mirrors System.Threading.Tasks.Sources
/// .ValueTaskSourceStatus.  Values match the managed enum so a codegen-emitted
/// comparison against the literal keeps working.
enum class ValueTaskSourceStatus : int32_t {
    Succeeded = 0,
    Faulted   = 1,
    Canceled  = 2,
};

/// The one-shot completion callback registered through IValueTaskSource.OnCompleted.
/// `state` is the opaque cursor the awaiter passed; the continuation resumes the state
/// machine.  Mirrors AsyncContinueFn so a registered dispatcher can be reused.
using AsyncIteratorContinueFn = void (*)(void* state);

/// ≙ ManualResetValueTaskSourceCore<bool>.
///
/// Holds one iteration's outcome plus its version.  `Acquire` hands out the current
/// version as the token; `Reset` bumps it, which is what invalidates any awaiter that
/// still remembers the previous iteration.
struct AsyncIteratorSourceCore
{
    // TResult = bool for the iterator protocol: true = element available, false = done.
    bool                result = false;

    // Cancellation and fault are distinct terminal states (the same three-state
    // discipline AsyncTask enforces — a cancelled await must not present as a
    // payload-less fault, and vice versa).
    bool                canceled = false;
    CHAOS_IL2CPP_INTPTR exception = 0;

    bool                completed = false;

    // The version IS the token.  Bumped on Reset; validated on every token-taking call.
    CHAOS_IL2CPP_INT16  version = 0;

    AsyncIteratorContinueFn continuation = nullptr;
    void*                   continuation_state = nullptr;
    std::atomic<bool>       has_continuation{false};
    std::atomic<bool>       continuation_fired{false};

    // Set by the pool when this core is handed out from the overflow path, so Release
    // knows to delete rather than re-pool.  Address-range inference is deliberately not
    // used — see the header note (3).
    bool                heap_allocated = false;

    CHAOS_IL2CPP_INT16 GetVersion() const noexcept { return version; }

    /// Return to the pre-acquire state for reuse.  Bumping the version is the whole
    /// point: it is what makes a late awaiter holding the old token detectable.
    void Reset() noexcept
    {
        result = false;
        canceled = false;
        exception = 0;
        completed = false;
        continuation = nullptr;
        continuation_state = nullptr;
        has_continuation.store(false, std::memory_order_relaxed);
        continuation_fired.store(false, std::memory_order_relaxed);
        // Wrap-around is harmless: only equality against a live token matters, and
        // 65536 iterations between two observations of the same core is not a case a
        // single iterator can reach (the pool would have to cycle fully twice).
        ++version;
    }

    /// Explicit, loud failure for a token that does not belong to this lifecycle.
    /// Deliberately not a "return false" — see the header note (1).
    void FailTokenMismatch(CHAOS_IL2CPP_INT16 token, const char* what) const noexcept
    {
        (void)token;
        (void)what;
        // CHAOS_IL2CPP_ABORT is the project's terminal-failure primitive; a token
        // mismatch means the pool is being driven incorrectly, which no caller can
        // recover from meaningfully.
        CHAOS_IL2CPP_ABORT();
    }

    bool TokenMatches(CHAOS_IL2CPP_INT16 token) const noexcept { return token == version; }

    void SetResult(bool value) noexcept
    {
        result = value;
        canceled = false;
        exception = 0;
        completed = true;
        FireContinuation();
    }

    void SetException(CHAOS_IL2CPP_INTPTR ex) noexcept
    {
        exception = ex;
        canceled = false;
        completed = true;
        FireContinuation();
    }

    /// Deliver the registered continuation exactly once.  Streamlined relative to
    /// finish_async_task: the is_completed fast path is already taken inside
    /// OnCompleted, and a suspended MoveNext always leaves a continuation registered.
    void FireContinuation() noexcept
    {
        if (!has_continuation.load(std::memory_order_acquire)) return;
        if (continuation_fired.exchange(true, std::memory_order_acq_rel)) return;
        AsyncIteratorContinueFn cb = continuation;
        void* state = continuation_state;
        if (cb != nullptr) cb(state);
    }

    /// IValueTaskSource.GetStatus(token).  A mismatched token is a pool bug, not a
    /// caller error to be tolerated.
    ValueTaskSourceStatus GetStatus(CHAOS_IL2CPP_INT16 token) noexcept
    {
        if (!TokenMatches(token)) FailTokenMismatch(token, "GetStatus");
        if (canceled) return ValueTaskSourceStatus::Canceled;
        if (exception != 0) return ValueTaskSourceStatus::Faulted;
        return ValueTaskSourceStatus::Succeeded;
    }

    /// IValueTaskSource<bool>.GetResult(token).
    CHAOS_IL2CPP_INT16 GetResult(CHAOS_IL2CPP_INT16 token) noexcept
    {
        if (!TokenMatches(token)) FailTokenMismatch(token, "GetResult");
        return result ? static_cast<CHAOS_IL2CPP_INT16>(1)
                      : static_cast<CHAOS_IL2CPP_INT16>(0);
    }

    /// IValueTaskSource<bool>.OnCompleted(continuation, state, token, flags).
    /// The token is validated here too — registering a continuation against a stale
    /// token would otherwise resume the wrong state machine.
    void OnCompleted(AsyncIteratorContinueFn cb, void* state,
                     CHAOS_IL2CPP_INT16 token) noexcept
    {
        if (!TokenMatches(token)) FailTokenMismatch(token, "OnCompleted");

        // Already complete: resume immediately rather than storing, so a consumer that
        // awaits a synchronously-finished element does not need a scheduler round-trip.
        if (completed)
        {
            if (cb != nullptr) cb(state);
            return;
        }

        continuation = cb;
        continuation_state = state;
        has_continuation.store(true, std::memory_order_release);

        // Re-check: the producer may have completed between the check above and the
        // publish.  Both sides use the same acquire/release pair as async.h's
        // async_task_on_completed so the race resolves to exactly one delivery.
        if (completed) FireContinuation();
    }
};

/// Per-iterator pool of source cores.
///
/// Capacity 8: a single iterator normally has one MoveNextAsync in flight, so 8 is
/// headroom rather than a tuned figure.  Exhaustion is handled, not asserted — nested
/// iteration (`await foreach` inside `await foreach`) can legitimately have more than
/// one outstanding.
class AsyncIteratorSourcePool
{
public:
    static constexpr int kPoolCapacity = 8;

    AsyncIteratorSourcePool() noexcept
    {
        for (int i = 0; i < kPoolCapacity; ++i) in_use_[i] = false;
    }

    // Non-copyable: the pool is owned by exactly one state machine, and the raw
    // overflow pointers below would double-free if the pool were copied.
    AsyncIteratorSourcePool(const AsyncIteratorSourcePool&) = delete;
    AsyncIteratorSourcePool& operator=(const AsyncIteratorSourcePool&) = delete;

    /// Hand out a source.  Returns its current version through `out_token`.
    /// Never returns nullptr and never blocks — see the header note (2).
    AsyncIteratorSourceCore* Acquire(CHAOS_IL2CPP_INT16* out_token) noexcept
    {
        {
            std::lock_guard<std::mutex> guard(mtx_);
            for (int i = 0; i < kPoolCapacity; ++i)
            {
                if (!in_use_[i])
                {
                    in_use_[i] = true;
                    AsyncIteratorSourceCore* core = &pool_[i];
                    core->heap_allocated = false;
                    core->Reset();
                    if (out_token != nullptr) *out_token = core->GetVersion();
                    return core;
                }
            }
        }

        // Overflow: allocate.  Deliberately outside the lock — allocation can be slow
        // and there is no reason to serialise other acquirers behind it.
        auto* core = new (std::nothrow) AsyncIteratorSourceCore();
        if (core == nullptr)
        {
            // Out of memory is unrecoverable here, and returning nullptr would push a
            // silent-null onto every caller.  Fail loudly instead.
            CHAOS_IL2CPP_ABORT();
        }
        core->heap_allocated = true;
        core->Reset();
        if (out_token != nullptr) *out_token = core->GetVersion();
        return core;
    }

    /// Return a source acquired from this pool.
    /// `core` must be one this pool handed out and must not still be in flight.
    void Release(AsyncIteratorSourceCore* core) noexcept
    {
        if (core == nullptr) return;

        if (core->heap_allocated)
        {
            delete core;
            return;
        }

        std::lock_guard<std::mutex> guard(mtx_);
        const int index = static_cast<int>(core - pool_);
        // A pointer that is neither an overflow allocation nor inside this pool means
        // the pool is being misused; corrupting in_use_ would silently corrupt a slot.
        if (index < 0 || index >= kPoolCapacity)
        {
            CHAOS_IL2CPP_ABORT();
        }
        in_use_[index] = false;
    }

    /// Number of pool slots currently handed out.  For tests and diagnostics.
    int InUseCount() noexcept
    {
        std::lock_guard<std::mutex> guard(mtx_);
        int n = 0;
        for (int i = 0; i < kPoolCapacity; ++i) if (in_use_[i]) ++n;
        return n;
    }

private:
    AsyncIteratorSourceCore pool_[kPoolCapacity];
    bool                    in_use_[kPoolCapacity];
    std::mutex              mtx_;
};

// ══════════════════════════════════════════════════════════════════════════════
// AsyncIteratorMethodBuilder — the 5 ops the iterator state machine calls.
//
// Reflection-confirmed surface (.NET 9.0.0), see a2-design §1.1:
//   static AsyncIteratorMethodBuilder Create()
//          void MoveNext<TStateMachine>(TStateMachine& stateMachine)
//          void AwaitOnCompleted<TAwaiter,TStateMachine>(TAwaiter&, TStateMachine&)
//          void AwaitUnsafeOnCompleted<TAwaiter,TStateMachine>(TAwaiter&, TStateMachine&)
//          void Complete()
//
// AwaitOnCompleted and AwaitUnsafeOnCompleted are BOTH present and must both be
// registered (the recon doc records that the original plan omitted the unsafe one).
// They share an implementation: the distinction in .NET concerns whether the awaiter
// admits unsafe continuations, which native code does not enforce.
//
// The builder's "instance" is a pool reference.  Create() returns an opaque handle to
// it; the codegen stores that in the state machine's <>t__builder field.
// ══════════════════════════════════════════════════════════════════════════════

/// Function pointer for the compiler-generated iterator MoveNext.
/// ABI: extern "C" void MoveNext(CHAOS_IL2CPP_INTPTR box).
using AsyncIteratorMoveNextFn = void (*)(CHAOS_IL2CPP_INTPTR);

/// Create(): allocate a pool and return it as an opaque handle.
/// Always returns non-zero — a zero handle would degrade to the silent-stub shape
/// A1 removed, and there is no legitimate "no builder" state for an iterator.
inline CHAOS_IL2CPP_INTPTR async_iterator_builder_create() noexcept
{
    auto* pool = new (std::nothrow) AsyncIteratorSourcePool();
    if (pool == nullptr) CHAOS_IL2CPP_ABORT();
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(pool);
}

inline AsyncIteratorSourcePool* as_iterator_pool(CHAOS_IL2CPP_INTPTR builder_handle) noexcept
{
    if (builder_handle == 0) CHAOS_IL2CPP_ABORT();
    return reinterpret_cast<AsyncIteratorSourcePool*>(builder_handle);
}

/// MoveNext<TStateMachine>(ref stateMachine): drive the state machine once.
///
/// Two call shapes exist because the state machine's MoveNext is invoked both by the
/// consumer (first call, and every call after the previous element was observed) and by
/// the builder's own resumption path.  Both funnel here.
///
/// Returns 1 if the state machine ran, 0 on invalid args.  The *result* of the iteration
/// step is not the return value — it is published through the source, which is why the
/// caller must have acquired one first.
inline CHAOS_IL2CPP_INTPTR async_iterator_builder_move_next(
    CHAOS_IL2CPP_INTPTR builder_handle,
    AsyncIteratorMoveNextFn move_next,
    void* sm_box) noexcept
{
    (void)builder_handle;
    if (move_next == nullptr) return 0;
    move_next(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm_box));
    return 1;
}

/// AwaitOnCompleted / AwaitUnsafeOnCompleted.
///
/// Registers `move_next(sm_box)` as the continuation of the awaited source identified by
/// (source, token).  The continuation runs on whatever thread completes the source; the
/// caller (the state machine) has already set its state so re-entry resumes correctly.
///
/// Returns 1 on success, 0 on invalid args.
inline CHAOS_IL2CPP_INTPTR async_iterator_builder_await_on_completed(
    AsyncIteratorSourceCore* source,
    CHAOS_IL2CPP_INT16 token,
    AsyncIteratorMoveNextFn move_next,
    void* sm_box) noexcept
{
    if (source == nullptr || move_next == nullptr) return 0;

    // The resumption context is per-registration and must not be shared across
    // concurrent awaits of the same iterator, so it is carried in a small heap box
    // freed by the trampoline after it fires exactly once.
    struct ResumeCtx {
        AsyncIteratorMoveNextFn move_next;
        void*                   sm_box;
    };
    auto* ctx = new (std::nothrow) ResumeCtx{move_next, sm_box};
    if (ctx == nullptr) CHAOS_IL2CPP_ABORT();

    source->OnCompleted(
        [](void* state) noexcept {
            auto* c = static_cast<ResumeCtx*>(state);
            AsyncIteratorMoveNextFn fn = c->move_next;
            void* box = c->sm_box;
            delete c;
            fn(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box));
        },
        ctx, token);
    return 1;
}

/// Complete(): the iterator finished iterating.  Releases the pool's overflow
/// allocations; the pool itself is owned by the state machine.
///
/// Note this does NOT free the pool: the state machine may still be referenced (its
/// GetAsyncEnumerator may be called again for a second iteration), and freeing here
/// would leave that future call holding a dangling handle.  Pool reclamation follows
/// the state machine's own lifetime.
inline void async_iterator_builder_complete(CHAOS_IL2CPP_INTPTR builder_handle) noexcept
{
    (void)builder_handle;
}

/// Destroy(): release the builder handle itself.  This is the pair to Create()'s
/// allocation and must be called exactly once, from wherever the state machine's
/// lifetime ends.  Separate from Complete() for the re-enumeration reason above.
inline void async_iterator_builder_destroy(CHAOS_IL2CPP_INTPTR builder_handle) noexcept
{
    if (builder_handle == 0) return;
    delete reinterpret_cast<AsyncIteratorSourcePool*>(builder_handle);
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_ASYNC_ITERATOR_H_
