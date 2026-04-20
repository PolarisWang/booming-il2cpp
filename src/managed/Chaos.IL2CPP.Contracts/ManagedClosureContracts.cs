namespace Chaos.IL2CPP.Contracts;

public sealed record ManagedClosureRequest(
    string InputAssemblyPath,
    string OutputRootPath,
    string? EntryPointSubjectIdOverride = null,
    IReadOnlyList<string>? AdditionalAssemblyPaths = null,
    bool FullAssemblyClosure = false);

public static class ManagedClosureArtifactNames
{
    public const string TypedIlIr = "typed-il-ir.json";
    public const string AotCoreIr = "aot-core-ir.json";
    public const string AotManifest = "aot-manifest.json";
    public const string MetadataRegistration = "metadata-registration.json";
    public const string SupplementalMetadataTemplate = "hot-update/supplemental-metadata-template.json";
    public const string CodeRegistration = "code-registration.json";
    public const string OptimizationFacts = "optimization-facts.json";
    public const string PreserveDescriptor = "preserve-descriptor.json";
    public const string NativeReferenceLoweringPlan = "native-reference.lowering-plan.json";
    public const string NativeAotLoweringPlan = "native-aot.lowering-plan.json";
    public const string ClosureManifest = "closure.manifest.json";
}
