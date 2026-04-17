using System.Collections.Generic;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Represents the compact collection file consumed by managed test hosts.
/// </summary>
public sealed class ChaosTestCollection
{
    /// <summary>
    /// Gets or sets the collection schema version.
    /// </summary>
    public int SchemaVersion { get; set; } = 1;

    /// <summary>
    /// Gets or sets the owning subject id.
    /// </summary>
    public string SubjectId { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets a value indicating whether the source assembly referenced the test framework.
    /// </summary>
    public bool FrameworkReferenced { get; set; }

    /// <summary>
    /// Gets or sets the high-level subject kind.
    /// </summary>
    public string SubjectKind { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the declaration scan warning codes.
    /// </summary>
    public List<string> WarningCodes { get; set; } = [];

    /// <summary>
    /// Gets or sets the declared proof entries.
    /// </summary>
    public List<ChaosTestCollectionEntry> DeclaredUnitTests { get; set; } = [];

    /// <summary>
    /// Gets or sets the declared benchmark entries.
    /// </summary>
    public List<ChaosTestCollectionEntry> DeclaredBenchmarks { get; set; } = [];
}

/// <summary>
/// Represents a single compact collection entry.
/// </summary>
public sealed class ChaosTestCollectionEntry
{
    /// <summary>
    /// Gets or sets the stable numeric entry index.
    /// </summary>
    public int EntryIndex { get; set; } = -1;

    /// <summary>
    /// Gets or sets the stable entry id.
    /// </summary>
    public string StableId { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the optional short alias.
    /// </summary>
    public string Alias { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the declaring assembly name.
    /// </summary>
    public string AssemblyName { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the fully-qualified declaring type.
    /// </summary>
    public string DeclaringType { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the static method name.
    /// </summary>
    public string MethodName { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the compact method signature.
    /// </summary>
    public string MethodSignature { get; set; } = string.Empty;
}
