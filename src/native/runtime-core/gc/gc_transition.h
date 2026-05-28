#ifndef CHAOS_IL2CPP_GC_TRANSITION_H_
#define CHAOS_IL2CPP_GC_TRANSITION_H_

// ── GC_TRANSITION macros ──────────────────────────────────────────
//
// Bridge between COOPERATIVE and PREEMPTIVE GC modes.
//
// Managed code runs in COOPERATIVE mode: threads must participate in
// GC safepoints by checking the generation counter.
//
// Native code (P/Invoke, blocking calls) runs in PREEMPTIVE mode:
// the thread is exempt from safepoint cooperation—CRAG conservative
// stack scanning handles root discovery.
//
// Usage in bridge stubs:
//   GC_TRANSITION_TO_PREEMPTIVE();
//   // ... native call ...
//   GC_TRANSITION_TO_COOPERATIVE();

#include "thread_state.h"
#include <chaos/compiler_hints.h>
#include <atomic>

namespace chaos::il2cpp::runtime_core {

constexpr uint32_t kGcModeCooperative = 0u;
constexpr uint32_t kGcModePreemptive  = 1u;

/// Transition from COOPERATIVE to PREEMPTIVE mode.
/// Must be called before entering native code that may block.
CHAOS_IL2CPP_FORCEINLINE void GcTransitionToPreemptive() noexcept {
    auto* thread = threading::tls_this_thread;
    if (thread == nullptr) return;
    thread->gc_mode.store(kGcModePreemptive, std::memory_order_release);
    // Participate in any in-progress safepoint before leaving managed context.
    threading::SafepointPoll();
}

/// Transition from PREEMPTIVE to COOPERATIVE mode.
/// Must be called after returning from native code.
CHAOS_IL2CPP_FORCEINLINE void GcTransitionToCooperative() noexcept {
    auto* thread = threading::tls_this_thread;
    if (thread == nullptr) return;
    thread->gc_mode.store(kGcModeCooperative, std::memory_order_release);
    // Re-check safepoint on re-entry to managed context.
    threading::SafepointPoll();
}

}  // namespace chaos::il2cpp::runtime_core

#define GC_TRANSITION_TO_PREEMPTIVE() \
    ::chaos::il2cpp::runtime_core::GcTransitionToPreemptive()

#define GC_TRANSITION_TO_COOPERATIVE() \
    ::chaos::il2cpp::runtime_core::GcTransitionToCooperative()

#endif  // CHAOS_IL2CPP_GC_TRANSITION_H_
