// ── Parallel stub declarations ────────────────────────────
// System.Threading.Tasks.Parallel.For / Parallel.Invoke
#pragma once

#include <chaos/native_types.h>

// Parallel.For(fromInclusive, toExclusive, bodyDelegate)
// bodyDelegate is an Action<int> (managed delegate reference).
// Returns 0 on success, non-zero on exception.
CHAOS_IL2CPP_INT32 ChaosParallelFor(
    CHAOS_IL2CPP_INT32 from_inclusive,
    CHAOS_IL2CPP_INT32 to_exclusive,
    CHAOS_IL2CPP_INTPTR body_delegate) noexcept;
