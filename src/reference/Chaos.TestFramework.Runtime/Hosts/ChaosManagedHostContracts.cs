namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Defines the managed host kinds supported by compact test hosts.
/// </summary>
public enum ChaosManagedHostKind : byte
{
    /// <summary>
    /// Declared proof host.
    /// </summary>
    Proof = 1,

    /// <summary>
    /// Declared benchmark host.
    /// </summary>
    Benchmark = 2,
}

/// <summary>
/// Represents a parsed managed host execution request.
/// </summary>
public readonly record struct ChaosManagedHostRequest(
    string CollectionPath,
    int EntryIndex);
