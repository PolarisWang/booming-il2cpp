namespace Chaos.IL2CPP.Contracts;

public sealed record ManagedClosureRequest(
    string InputAssemblyPath,
    string OutputRootPath,
    string? EntryPointSubjectIdOverride = null,
    IReadOnlyList<string>? AdditionalAssemblyPaths = null,
    bool FullAssemblyClosure = false)
{
    /// <summary>
    /// Optional set of SubjectIds to limit lowering to only these methods
    /// plus their direct callees. When null or empty, all methods in the
    /// assembly are lowered (default behavior).
    /// </summary>
    public IReadOnlySet<string>? SubjectMethodIds { get; init; }
}

/// <summary>
/// Request to process multiple assemblies in a single pipeline run.
/// All assemblies are loaded into a unified semantic world, linked together,
/// and produce per-assembly codegen outputs.
/// </summary>
public sealed record MultiAssemblyClosureRequest(
    IReadOnlyList<string> InputAssemblyPaths,
    string OutputRootPath,
    string? EntryPointSubjectIdOverride = null,
    IReadOnlyList<string>? AdditionalAssemblyPaths = null);

public static class ManagedClosureArtifactNames
{
    public const string TypedIlIr = "typed-il-ir.json";
    public const string AotCoreIr = "aot-core-ir.json";
    public const string AotManifest = "aot-manifest.json";
    public const string MetadataRegistration = "metadata-registration.json";
    public const string SupplementalMetadataTemplate = "hot-update/supplemental-metadata-template.json";
    public const string SupplementalMetadata = "hot-update/supplemental-metadata.json";
    public const string CodeRegistration = "code-registration.json";
    public const string GenericInstantiationDemandGraph = "generic-instantiation-demand-graph.json";
    public const string GenericCapabilityMatrix = "generic-capability-matrix.json";
    public const string OptimizationFacts = "optimization-facts.json";
    public const string PreserveDescriptor = "preserve-descriptor.json";
    public const string NativeReferenceLoweringPlan = "native-reference.lowering-plan.json";
    public const string NativeAotLoweringPlan = "native-aot.lowering-plan.json";
    public const string ClosureManifest = "closure.manifest.json";
}
