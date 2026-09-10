namespace Chaos.IL2CPP.Contracts;

public sealed record LoadedWorldModel
{
    public required string InputAssemblyPath { get; init; }

    public required bool FullAssemblyClosure { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    /// <summary>
    /// The entry point SubjectId resolved by the pipeline.
    /// In FullAssemblyClosure mode this is deliberately empty (string.Empty) so the
    /// codegen skeleton-plan discriminator <c>FullAssemblyClosure && string.IsNullOrWhiteSpace(EntryPointSubjectId)</c>
    /// continues to select the full-skeleton emit path.  The real entry point
    /// (resolved from the PE token or an override) is always carried in
    /// <see cref="ResolvedEntryPointSubjectId"/> for downstream consumers like
    /// publish-mode app_main.cpp generation.
    /// </summary>
    public required string EntryPointSubjectId { get; init; }

    /// <summary>The resolved entry point, always set regardless of FullAssemblyClosure.</summary>
    public string? ResolvedEntryPointSubjectId { get; init; }

    public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }

    public required IReadOnlyList<LoadedAssemblyModel> Assemblies { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }
}
