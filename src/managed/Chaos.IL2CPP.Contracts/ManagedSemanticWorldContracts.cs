namespace Chaos.IL2CPP.Contracts;

public sealed record CapabilityBundlesModel
{
    public required WorldCapabilityBundleModel World { get; init; }

    public required IReadOnlyList<MethodCapabilityBundleModel> Methods { get; init; }
}

public sealed record WorldCapabilityBundleModel
{
    public required IReadOnlyList<string> Capabilities { get; init; }
}

public sealed record MethodCapabilityBundleModel
{
    public required string SubjectId { get; init; }

    public required IReadOnlyList<string> Capabilities { get; init; }
}

public sealed record SemanticWorldModel
{
    public required string InputAssemblyPath { get; init; }

    public required bool FullAssemblyClosure { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    public required IReadOnlyList<AssemblyIdentityModel> Assemblies { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }

    public required CanonicalSubjectsModel CanonicalSubjects { get; init; }

    public required SemanticShapesModel SemanticShapes { get; init; }

    public required CapabilityBundlesModel CapabilityBundles { get; init; }
}

public sealed record LinkedWorldModel
{
    public required string InputAssemblyPath { get; init; }

    public required bool FullAssemblyClosure { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    public required IReadOnlyList<AssemblyIdentityModel> Assemblies { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }

    public required IReadOnlyList<LinkedDependencyModel> Dependencies { get; init; }

    public required CanonicalSubjectsModel CanonicalSubjects { get; init; }

    public required SemanticShapesModel SemanticShapes { get; init; }

    public required CapabilityBundlesModel CapabilityBundles { get; init; }

    public required OptimizationFactsArtifact OptimizationFacts { get; init; }

    public required PreserveDescriptorArtifact PreserveDescriptor { get; init; }
}

public sealed record LinkedDependencyModel
{
    public required string AssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string Reason { get; init; }
}

public sealed record OptimizationFactsArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "optimizationFacts";

    public required IReadOnlyList<ClosedWorldSpecializationFact> ClosedWorldSpecializations { get; init; }

    public required IReadOnlyList<DispatchFact> DispatchFacts { get; init; }

    public required IReadOnlyList<LayoutFact> LayoutFacts { get; init; }

    public required IReadOnlyList<ExceptionFact> ExceptionFacts { get; init; }
}

public sealed record PreserveDescriptorArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "preserveDescriptor";

    public required IReadOnlyList<PreserveDescriptorEntry> Entries { get; init; }
}

public sealed record PreserveDescriptorEntry
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string Preserve { get; init; }

    public required string Reason { get; init; }
}

public sealed record ClosedWorldSpecializationFact
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string Reason { get; init; }
}

public sealed record DispatchFact
{
    public required string MethodSubjectId { get; init; }

    public required string DispatchKind { get; init; }

    public required string TargetSubjectId { get; init; }

    public bool Devirtualized { get; init; }
}

public sealed record LayoutFact
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string DataKind { get; init; }

    public string? ElementType { get; init; }

    public bool RequiresBoxing { get; init; }
}

public sealed record ExceptionFact
{
    public required string MethodSubjectId { get; init; }

    public required string HandlingKind { get; init; }

    public string? CatchTypeSubjectId { get; init; }
}

