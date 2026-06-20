// StringBuilder pool for codegen emission.
// Each method emission allocates a StringBuilder(4096).  Pooling them
// across thousands of methods reduces GC pressure (#gen0 collections)
// and avoids repeated large-buffer allocations on the LOH.
//
// Usage:
//   var sb = StringBuilderPool.Rent(4096);
//   // ... use sb ...
//   string result = sb.ToString();
//   StringBuilderPool.Return(sb);
//
// Design:
//   - ThreadLocal<StringBuilder?> so each thread caches at most ONE builder.
//     This is sufficient because the emission loop is naturally sequential
//     within a thread (one method at a time).
//   - Only builders with capacity <= 64KB are cached; larger buffers are
//     discarded after use to avoid holding onto huge allocations.
//   - Return() calls sb.Clear() before stashing (avoids O(n) clear cost
//     if the caller already called ToString() — but since Clear is O(1)
//     on StringBuilder it's negligible).
//   - Not a full object pool (no ConcurrentBag, no trimming thread).
//     ThreadLocal is zero-overhead on the hot path.

using System.Text;

namespace Chaos.IL2CPP.Generator;

internal static class StringBuilderPool
{
    /// <summary>Maximum capacity (bytes) of a cached StringBuilder.</summary>
    private const int MaxCachedCapacity = 65536; // 64 KB

    [ThreadStatic]
    private static StringBuilder? t_cached;

    /// <summary>Rent a StringBuilder.  Returns a cached one if available (already cleared),
    /// otherwise allocates a new one with the given initial capacity.</summary>
    public static StringBuilder Rent(int initialCapacity = 4096)
    {
        var sb = t_cached;
        if (sb != null)
        {
            t_cached = null;
            sb.Clear();
            if (sb.Capacity < initialCapacity)
                sb.Capacity = initialCapacity;
            return sb;
        }
        return new StringBuilder(initialCapacity);
    }

    /// <summary>Return a StringBuilder to the pool.  Only caches builders
    /// with capacity <= MaxCachedCapacity.</summary>
    public static void Return(StringBuilder sb)
    {
        if (sb.Capacity > MaxCachedCapacity)
            return; // too large — let GC reclaim it
        sb.Clear();
        t_cached = sb;
    }
}
