#ifndef CHAOS_IL2CPP_SYNCHRONIZATION_CONTEXT_H_
#define CHAOS_IL2CPP_SYNCHRONIZATION_CONTEXT_H_

// synchronization_context.h — SynchronizationContext + its dispatch surface.
//
// WHAT THIS IS FOR
// ----------------
// `await x.ConfigureAwait(true)` (and any awaiter whose continuation must return
// to "the context it started on") resolves SynchronizationContext.Current when
// the awaiter completes.  With no context installed the continuation runs on
// whatever thread completed the task — which is what ConfigureAwait(false)
// asks for, and is the correct default for a headless AOT runtime.
//
// WHAT THIS DELIBERATELY IS NOT
// -----------------------------
// There is no UI message pump here and there is no plan to add one.  The
// minimal context this file provides is a *transport*, not a scheduler:
//
//   Post  -> queue the callback on the ThreadPool  (async, does not block)
//   Send  -> invoke the callback inline            (sync, no pump to marshal to)
//
// `Send` being inline is the honest choice: the alternative is blocking the
// calling thread until a pump that does not exist drains the callback.  A
// caller that requires true UI-thread affinity needs a real context installed
// via SetSynchronizationContext; the placeholder returned by `new` does not
// claim to provide it.
//
// WHY .Current STILL MATTERS
// --------------------------
// Even without a pump, `.Current` must faithfully reflect what was Set on this
// thread, because the *decision* of where to resume is made from it.  A
// SetSynchronizationContext that silently discards its argument makes
// ConfigureAwait(true) and ConfigureAwait(false) indistinguishable — the two
// would behave identically and the acceptance counter-example ("resume thread
// != original thread" for ConfigureAwait(false)) could not tell them apart.

#include <chaos/native_types.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Opaque context handle.  A managed SynchronizationContext is represented by
/// this handle; the runtime owns the underlying object.
struct SynchronizationContext {
    /// True for the placeholder a plain `new SynchronizationContext()` creates.
    /// Such a context posts to the ThreadPool and sends inline — see the file
    /// comment for why that is the correct default rather than a stub.
    bool is_placeholder{true};
};

/// Create a placeholder context.  Returns nullptr on allocation failure.
SynchronizationContext* SynchronizationContextCreate() noexcept;

/// Destroy a context created by SynchronizationContextCreate.  Null-safe.
void SynchronizationContextDestroy(SynchronizationContext* ctx) noexcept;

/// Install `ctx` as the current thread's context.  Pass nullptr to clear it.
/// Returns the PREVIOUS context so a caller can restore it.
SynchronizationContext* SynchronizationContextSetCurrent(SynchronizationContext* ctx) noexcept;

/// The current thread's context, or nullptr when none is installed.
SynchronizationContext* SynchronizationContextGetCurrent() noexcept;

/// Invoke `callback(state)` asynchronously on the ThreadPool.
/// Returns false when the callback could not be queued.
bool SynchronizationContextPost(SynchronizationContext* ctx,
                                void (*callback)(void*), void* state) noexcept;

/// Invoke `callback(state)` synchronously, inline on the calling thread.
/// Returns false when the callback could not be invoked.
bool SynchronizationContextSend(SynchronizationContext* ctx,
                                void (*callback)(void*), void* state) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

// ── extern "C" bridges for codegen-emitted C++ ──
extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_create() noexcept;
extern "C" void chaos_synchronization_context_destroy(CHAOS_IL2CPP_INTPTR ctx) noexcept;
extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_get_current() noexcept;
extern "C" CHAOS_IL2CPP_INTPTR chaos_synchronization_context_set_current(CHAOS_IL2CPP_INTPTR ctx) noexcept;

#endif  // CHAOS_IL2CPP_SYNCHRONIZATION_CONTEXT_H_
