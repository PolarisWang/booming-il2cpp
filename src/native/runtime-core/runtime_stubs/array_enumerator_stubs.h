// ── Array enumerator support (T6) ──────────────────────────────────────────
//
// Assert.AreEqual<T> in Chaos.TestFramework.Sdk compares two IEnumerable
// operands by walking GetEnumerator()/MoveNext()/get_Current().  For array
// operands those three calls reach the AOT external-runtime table, where
// they previously had no native body: each helper returned 0, so the
// enumeration loop terminated on the first MoveNext and *every* collection
// comparison reported a mismatch — even when both sides were the same empty
// array.  The receiver is supplied by the codegen (see the T6 receiver
// injection in NativeAotLoweringPlanner.ExternalRuntimeHelpers); these
// implementations consume it.
//
// The enumerator is a small GC-allocated native object holding the source
// array plus a cursor.  It is deliberately *not* a managed object: nothing in
// the managed world observes its type identity, the three call sites only
// round-trip it through get_Current/MoveNext.  It is kept alive by the fact
// dispatch frame for the duration of the comparison.

#pragma once

#include <chaos/native_types.h>

extern "C" {

/// IEnumerable.GetEnumerator() over a managed 1-D array.
/// Returns an opaque enumerator handle, or 0 when `array` is null.
CHAOS_IL2CPP_INTPTR ChaosArrayGetEnumerator(CHAOS_IL2CPP_INTPTR array) noexcept;

/// IEnumerator.MoveNext() — advances the cursor.
/// Returns 1 when a further element exists, 0 otherwise.
CHAOS_IL2CPP_INTPTR ChaosArrayEnumeratorMoveNext(CHAOS_IL2CPP_INTPTR enumerator) noexcept;

/// IEnumerator.get_Current() — the element at the cursor.
/// Returns 0 when the enumerator is exhausted or the cursor is out of range.
CHAOS_IL2CPP_INTPTR ChaosArrayEnumeratorGetCurrent(CHAOS_IL2CPP_INTPTR enumerator) noexcept;

}  // extern "C"
