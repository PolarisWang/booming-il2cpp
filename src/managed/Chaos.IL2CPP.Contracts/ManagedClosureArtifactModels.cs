namespace Chaos.IL2CPP.Contracts;

public sealed record AotManifestArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "aotManifest";

    public required IReadOnlyList<AotManifestEntry> Entries { get; init; }
}

public sealed record AotManifestEntry
{
    public required string AssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string Reason { get; init; }
}

public sealed record MetadataRegistrationArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "metadataRegistration";

    public required IReadOnlyList<MetadataRegistrationEntry> Registrations { get; init; }
}

public sealed record MetadataRegistrationEntry
{
    public required string RegistrationKind { get; init; }

    public required int Slot { get; init; }

    public required string SubjectId { get; init; }

    public string? Name { get; init; }

    public string? NamespaceName { get; init; }

    public string? DisplayName { get; init; }

    public string? DefinitionSubjectId { get; init; }

    public string? DeclaringTypeSubjectId { get; init; }

    public string? DeclaringMethodSubjectId { get; init; }

    public string? MemberType { get; init; }

    public int? ParameterIndex { get; init; }

    public int? ParameterCount { get; init; }

    public bool? IsImported { get; init; }

    public string? ImportModuleName { get; init; }

    public string? ImportEntryPointName { get; init; }
}

public sealed record SupplementalMetadataTemplateArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "supplementalMetadataTemplate";

    public required IReadOnlyList<SupplementalMetadataTypeTemplateEntry> RegisteredTypes { get; init; }

    public required IReadOnlyList<SupplementalMetadataMethodTemplateEntry> RegisteredMethods { get; init; }

    public required SupplementalMetadataReservedSlots ReservedSlots { get; init; }
}

public sealed record SupplementalMetadataTypeTemplateEntry
{
    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public GenericContextArtifact? GenericContext { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record SupplementalMetadataMethodTemplateEntry
{
    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public GenericContextArtifact? GenericContext { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required int MetadataToken { get; init; }

    public required int ParameterCount { get; init; }
}

public sealed record SupplementalMetadataReservedSlots
{
    public int TypeCount { get; init; } = 256;

    public int MethodCount { get; init; } = 1024;

    public int GenericInstantiationCount { get; init; } = 256;
}

public sealed record CodeRegistrationArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "codeRegistration";

    public required IReadOnlyList<CodeRegistrationModule> Modules { get; init; }
}

public sealed record CodeRegistrationModule
{
    public required string ModuleName { get; init; }

    public required IReadOnlyList<CodeRegistrationEntry> Registrations { get; init; }
}

public sealed record CodeRegistrationEntry
{
    public required string RegistrationKind { get; init; }

    public required int Slot { get; init; }

    public required string Symbol { get; init; }

    public required string SubjectId { get; init; }
}

public sealed record MetadataWriterOutput
{
    public required AotManifestArtifact AotManifest { get; init; }

    public required MetadataRegistrationArtifact MetadataRegistration { get; init; }

    public required SupplementalMetadataTemplateArtifact SupplementalMetadataTemplate { get; init; }
}

public sealed record ManagedClosureArtifactRef
{
    public required string Kind { get; init; }

    public required string Path { get; init; }
}

public sealed record ManagedClosureManifestArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "managedClosureManifest";

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string InputAssemblyPath { get; init; }

    public IReadOnlyList<string>? AdditionalAssemblyPaths { get; init; }

    public required string InputModuleVersionId { get; init; }

    public required IReadOnlyList<ManagedClosureArtifactRef> Artifacts { get; init; }
}

public sealed record ManagedClosureResult
{
    public required string OutputRootPath { get; init; }

    public required TypedIlIrArtifact TypedIlIr { get; init; }

    public required AotCoreIrArtifact AotCoreIr { get; init; }

    public required AotManifestArtifact AotManifest { get; init; }

    public required MetadataRegistrationArtifact MetadataRegistration { get; init; }

    public required SupplementalMetadataTemplateArtifact SupplementalMetadataTemplate { get; init; }

    public required CodeRegistrationArtifact CodeRegistration { get; init; }

    public required OptimizationFactsArtifact OptimizationFacts { get; init; }

    public required PreserveDescriptorArtifact PreserveDescriptor { get; init; }

    public required NativeReferenceLoweringPlanArtifact NativeReferenceLoweringPlan { get; init; }

    public required NativeAotLoweringPlanArtifact NativeAotLoweringPlan { get; init; }

    public required ManagedClosureManifestArtifact ClosureManifest { get; init; }
}

public sealed record NativeReferenceProofRequest(string ManagedClosureRootPath, string OutputRootPath);

public sealed record NativeAotRequest(string ManagedClosureRootPath, string OutputRootPath);

public static class NativeReferenceArtifactNames
{
    public const string GeneratedDirectory = "generated";
    public const string GeneratedTranslationUnit = "generated/native-reference.generated.cpp";
    public const string LoweringPlan = "native-reference.plan.json";
    public const string Manifest = "native-reference.manifest.json";
}

public static class NativeAotArtifactNames
{
    public const string GeneratedDirectory = "generated";
    public const string GeneratedTranslationUnit = "generated/native-aot.generated.cpp";
    public const string LoweringPlan = "native-aot.plan.json";
    public const string Manifest = "native-aot.manifest.json";
}

public sealed record NativeReferenceGeneratedArtifactRef
{
    public required string Kind { get; init; }

    public required string Path { get; init; }
}

public sealed record NativeReferenceProofManifestArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeReferenceManifest";

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string ManagedClosureRootPath { get; init; }

    public required string PlanArtifactPath { get; init; }

    public required IReadOnlyList<NativeReferenceGeneratedArtifactRef> GeneratedArtifacts { get; init; }
}

public sealed record NativeAotGeneratedArtifactRef
{
    public required string Kind { get; init; }

    public required string Path { get; init; }
}

public sealed record NativeAotManifestArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeAotManifest";

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string ManagedClosureRootPath { get; init; }

    public required string PlanArtifactPath { get; init; }

    public required IReadOnlyList<NativeAotGeneratedArtifactRef> GeneratedArtifacts { get; init; }
}

public sealed record EngineBindingsArtifact
{
    public required string ProofKind { get; init; }

    public required string FocusArea { get; init; }

    public required IReadOnlyList<string> CapabilityIds { get; init; }

    public required IReadOnlyList<string> BindingKinds { get; init; }

    public required IReadOnlyList<string> HelperNames { get; init; }
}

public sealed record HostBindingsArtifact
{
    public required string HostPlatform { get; init; }

    public required string RuntimeProfile { get; init; }

    public required IReadOnlyList<string> BindingKinds { get; init; }
}

public sealed record NativeReferenceLoweringPlanArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeReferenceLoweringPlan";

    public required string PlanKind { get; init; }

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string IncludeHeader { get; init; }

    public required string NativeEntryFunctionName { get; init; }

    public required string EntrySymbol { get; init; }

    public string? ConstructorSymbol { get; init; }

    public string? InstanceMethodSymbol { get; init; }

    public string? EchoMethodSymbol { get; init; }

    public string? GetterSymbol { get; init; }

    public string? StaticMethodSymbol { get; init; }

    public string? ThrowMethodSymbol { get; init; }

    public required string ReferenceTypeToken { get; init; }

    public required string CapturedFieldToken { get; init; }

    public required string EntryMethodToken { get; init; }

    public string? ConstructorMethodToken { get; init; }

    public string? InstanceMethodToken { get; init; }

    public string? DispatchStrategy { get; init; }

    public string? EchoMethodToken { get; init; }

    public string? GetterMethodToken { get; init; }

    public string? StaticMethodToken { get; init; }

    public string? ThrowMethodToken { get; init; }

    public string? ImportMethodSubjectId { get; init; }

    public string? ImportMethodSymbol { get; init; }

    public string? ImportModuleName { get; init; }

    public string? ImportEntryPointName { get; init; }

    public int? ImportArgument0 { get; init; }

    public int? ImportArgument1 { get; init; }

    public int? ImportArgument2 { get; init; }

    public required string ConsoleWriteLineStringIcall { get; init; }

    public string? StringConcatPairIcall { get; init; }

    public string? ConstructorLiteral { get; init; }

    public int? ConstructorLiteralByteCount { get; init; }

    public string? MessagePrefixLiteral { get; init; }

    public int? MessagePrefixLiteralByteCount { get; init; }

    public string? MessageSuffixLiteral { get; init; }

    public int? MessageSuffixLiteralByteCount { get; init; }

    public string? EchoLiteral { get; init; }

    public int? EchoLiteralByteCount { get; init; }

    public string? FinallyLiteral { get; init; }

    public int? FinallyLiteralByteCount { get; init; }

    public string? BoxedValueTypeToken { get; init; }

    public int? BoxedInt32Value { get; init; }

    public string? ClosedTypeSubjectId { get; init; }

    public string? GenericTypeDefinitionSubjectId { get; init; }

    public string? FieldSubjectId { get; init; }

    public string? PropertySubjectId { get; init; }

    public string? MethodSubjectId { get; init; }

    public string? ParameterSubjectId { get; init; }

    public string? ClosedTypeToken { get; init; }

    public string? GenericTypeDefinitionToken { get; init; }

    public string? ClosedTypeNamespaceName { get; init; }

    public string? ClosedTypeName { get; init; }

    public string? ClosedTypeDisplayName { get; init; }

    public string? GenericTypeDefinitionName { get; init; }

    public string? FieldQueryName { get; init; }

    public string? PropertyQueryName { get; init; }

    public string? MethodQueryName { get; init; }

    public int? MethodParameterCount { get; init; }

    public int? ParameterIndex { get; init; }

    public string? OutputPrefix { get; init; }

    public string? ExpectedOutput { get; init; }

    public int? ExpectedOutputByteCount { get; init; }

    public EngineBindingsArtifact? EngineBindings { get; init; }

    public HostBindingsArtifact? HostBindings { get; init; }
}

public sealed record NativeAotLoweringPlanArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeAotLoweringPlan";

    public required string PlanKind { get; init; }

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string NativeEntryFunctionName { get; init; }

    public required string EntrySymbol { get; init; }

    public required string EntryMethodToken { get; init; }

    public required string WorkloadAbi { get; init; }
}

public sealed record NativeReferenceGeneratedSource
{
    public required string RelativePath { get; init; }

    public required string Contents { get; init; }
}

public sealed record NativeAotGeneratedSource
{
    public required string RelativePath { get; init; }

    public required string Contents { get; init; }
}

public sealed record NativeReferenceProofResult
{
    public required string OutputRootPath { get; init; }

    public required NativeReferenceLoweringPlanArtifact LoweringPlan { get; init; }

    public required NativeReferenceProofManifestArtifact Manifest { get; init; }

    public required IReadOnlyList<NativeReferenceGeneratedSource> GeneratedSources { get; init; }
}

public sealed record NativeAotResult
{
    public required string OutputRootPath { get; init; }

    public required NativeAotLoweringPlanArtifact LoweringPlan { get; init; }

    public required NativeAotManifestArtifact Manifest { get; init; }

    public required IReadOnlyList<NativeAotGeneratedSource> GeneratedSources { get; init; }
}
