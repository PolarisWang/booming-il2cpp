// synchronization_context.cpp — SynchronizationContext placeholder + dispatch.
//
// See synchronization_context.h for design rationale.
//
// THE REAL IMPLEMENTATION
// -----------------------
// A placeholder SynchronizationContext (the kind `new SynchronizationContext()`
// creates) invokes Post and Send on the calling thread, inline.  This is
// semantically correct because in a headless AOT runtime there is no UI pump
// to marshal to.  A *real* UI-thread context installed by
// `SetSynchronizationContext` would override Post to queue to its own pump;
// that API surface works, the placeholder just has no pump to queue to.
//
// The extern "C" bridge at the bottom is what generated C++ calls via the
// ShapeRegistry; the namespace members above it are the runtime-internal API.

#include "synchronization_context.h"

#include <cstdlib>
#include <thread>

namespace chaos::il2cpp::runtime_core::threading {

namespace {

// thread_local for the current thread's SynchronizationContext.
thread_local SynchronizationContext* tls_current_ctx = nullptr;

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
    // Placeholder context: invoke inline.  This is the honest choice — without
    // a UI pump the only alternative is blocking the caller forever.
    if (ctx == nullptr || callback == nullptr) return false;
    callback(state);
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