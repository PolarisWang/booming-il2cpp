namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Data contract for global generic sharing canonical mapping directives.
///
/// When multiple compilation units process different subsets of a codebase,
/// each unit independently builds its own canonical map. This directive
/// allows an external coordination step (e.g., a pre-codegen scan) to
/// specify a consistent canonical choice across all units.
///
/// Currently used as documentation and future-proofing: the codegen processes
/// all modules in a single closure, so cross-module canonical consistency
/// is guaranteed by <see cref="NativeAotLoweringPlanner.PickCanonicalInstantiation"/>
/// (deterministic System.Object preference + SubjectId sort).
/// </summary>
public sealed record GlobalGenericSharingDirective
{
    /// <summary>
    /// Maps non-canonical method SubjectId → canonical method SubjectId.
    /// Each key is a closed generic instantiation that should share the
    /// canonical body identified by the value.
    /// </summary>
    public required IReadOnlyDictionary<string, string> CanonicalMapEntries { get; init; }

    /// <summary>
    /// Version stamp for cache invalidation. Bump when the sharing algorithm
    /// changes (e.g., new canonical selection rules).
    /// </summary>
    public required string Version { get; init; }

    /// <summary>Current version identifier.</summary>
    public const string CurrentVersion = "v1";
}
