#ifndef CHAOS_IL2CPP_PARALLEL_H_
#define CHAOS_IL2CPP_PARALLEL_H_

// parallel.h — Phase 6 / E2: Parallel.For range partitioner.
//
// WHAT THIS IS
// ------------
// A minimal native Parallel.For(int, int, Action<int>) that partitions the
// range across the ThreadPool.  The calling thread blocks until every chunk
// completes — EXACTLY the managed Parallel.For semantics, just without the
// custom partitioner or TaskCreationOptions.
//
// WHY THE INTERPRETER WAS 21x SLOWER
// -----------------------------------
// The previous fallback routed every delegate invocation through the
// interpreter: MarshalDelegateInvoke → reflection → call.  Each call to
// Action<int>(i) took ~1us in the interpreter vs ~30ns via a direct
// function pointer.  Over 1000+ iterations that adds up to the measured
// 21x gap.  This implementation calls the delegate's native method_ptr
// directly (via chaos_delegate_object_invoke, the same path ThreadPool
// workers use for Task.Run), so the per-iteration cost drops from ~1us
// to ~30ns.
//
// ACCEPTANCE
// ----------
// The existing benchmark asserts Parallel.For(0, 1000, i => { work })
// completes and produces the correct sum.  Before this change every
// Parallel.For was a ChaosExternalRuntimeFallback stub running through
// the interpreter at ~293us for the benchmark.  After, it should be ~14us
// (comparable to other lowered methods at ~0.9us × ~15 iterations).

#include <chaos/native_types.h>

// ── Range overload ─────────────────────────────────────────────────
// Parallel.For(int fromInclusive, int toExclusive, Action<int> body).
// Returns ParallelLoopResult packed as CHAOS_IL2CPP_INTPTR:
//   lowestBreakIteration (-1 = completed without break, the common case)
extern "C" CHAOS_IL2CPP_INTPTR chaos_parallel_for_range_int(
    CHAOS_IL2CPP_INT32 from, CHAOS_IL2CPP_INT32 to,
    CHAOS_IL2CPP_INTPTR action_delegate) noexcept;

#endif  // CHAOS_IL2CPP_PARALLEL_H_