using System.Text;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Thread-local StringBuilder pool that reduces GC pressure from per-method
/// StringBuilder allocations during codegen.
///
/// Usage:
///   var sb = StringBuilderPool.Rent(capacity);
///   // ... use sb ...
///   var result = sb.ToString();
///   StringBuilderPool.Return(sb);
///   return result;
///
/// Only builders up to <see cref="MaxCacheCapacity"/> are pooled to avoid
/// holding on to multi-MB temporary buffers.
/// </summary>
internal static class StringBuilderPool
{
    private const int MaxCacheCapacity = 65536;

    [ThreadStatic]
    private static StringBuilder? _cached;

    /// <summary>
    /// Rent a StringBuilder from the pool, or create one if the pool is empty.
    /// The returned builder is cleared and ready for use.
    /// </summary>
    public static StringBuilder Rent(int initialCapacity = 4096)
    {
        var sb = _cached;
        if (sb != null)
        {
            _cached = null;
            sb.Clear();
            if (sb.Capacity < initialCapacity)
                sb.Capacity = initialCapacity;
            return sb;
        }
        return new StringBuilder(initialCapacity);
    }

    /// <summary>
    /// Return a StringBuilder to the pool. Do not use the builder after returning it.
    /// </summary>
    public static void Return(StringBuilder sb)
    {
        if (sb.Capacity <= MaxCacheCapacity)
        {
            sb.Clear();
            _cached = sb;
        }
    }
}
