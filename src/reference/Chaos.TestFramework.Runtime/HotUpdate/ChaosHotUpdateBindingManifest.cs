using System.Collections.Generic;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Represents the compact hotupdate binding manifest consumed by generated hosts.
/// </summary>
public sealed class ChaosHotUpdateBindingManifest
{
    /// <summary>
    /// Gets or sets the subject id.
    /// </summary>
    public string SubjectId { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the collection path associated with the binding manifest.
    /// </summary>
    public string CollectionPath { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the patch project ids participating in the host.
    /// </summary>
    public List<string> PatchProjectIds { get; set; } = [];

    /// <summary>
    /// Gets or sets the patch assembly names that should be preloaded.
    /// </summary>
    public List<string> PatchAssemblyNames { get; set; } = [];

    /// <summary>
    /// Gets or sets the compact entry bindings.
    /// </summary>
    public List<ChaosHotUpdateEntryBinding> EntryBindings { get; set; } = [];
}

/// <summary>
/// Represents a compact entry-to-assembly binding.
/// </summary>
public sealed class ChaosHotUpdateEntryBinding
{
    /// <summary>
    /// Gets or sets the managed host kind.
    /// </summary>
    public string HostKind { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the declared entry index.
    /// </summary>
    public int EntryIndex { get; set; } = -1;

    /// <summary>
    /// Gets or sets the target assembly name.
    /// </summary>
    public string AssemblyName { get; set; } = string.Empty;

    /// <summary>
    /// Gets or sets the stable id when available.
    /// </summary>
    public string StableId { get; set; } = string.Empty;
}
