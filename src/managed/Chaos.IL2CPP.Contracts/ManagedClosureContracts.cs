namespace Chaos.IL2CPP.Contracts;

public sealed record ManagedClosureRequest(
    string InputAssemblyPath,
    string OutputRootPath,
    string? EntryPointSubjectIdOverride = null);

public static class ManagedClosureArtifactNames
{
    public const string TypedIlIr = "typed-il-ir.json";
    public const string AotManifest = "aot-manifest.json";
    public const string MetadataRegistration = "metadata-registration.json";
    public const string CodeRegistration = "code-registration.json";
    public const string OptimizationFacts = "optimization-facts.json";
    public const string NativeReferenceLoweringPlan = "native-reference.lowering-plan.json";
    public const string ClosureManifest = "closure.manifest.json";
}

public static class ManagedNaming
{
    public static string CreateTypeSubjectId(string assemblyName, string? namespaceName, string typeName)
    {
        return $"{assemblyName}/{GetTypeIdentityPart(assemblyName, namespaceName, typeName)}";
    }

    public static string CreateTypeDisplayName(string assemblyName, string? namespaceName, string typeName)
    {
        return string.Equals(namespaceName, assemblyName, StringComparison.Ordinal) || string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    public static string CreateFieldSubjectId(string declaringTypeSubjectId, string fieldName)
    {
        return $"{declaringTypeSubjectId}::{fieldName}";
    }

    public static string CreatePropertySubjectId(string declaringTypeSubjectId, string propertyName)
    {
        return $"{declaringTypeSubjectId}::property:{propertyName}";
    }

    public static string CreateMethodSubjectId(
        string declaringTypeSubjectId,
        string methodName,
        IReadOnlyList<string> parameterTypes)
    {
        return $"{declaringTypeSubjectId}::{methodName}({string.Join(",", parameterTypes)})";
    }

    public static string CreateParameterSubjectId(string methodSubjectId, int parameterIndex, string parameterName)
    {
        return $"{methodSubjectId}::parameter[{parameterIndex}]:{parameterName}";
    }

    public static string CreateMethodSignature(
        string returnType,
        string declaringTypeDisplayName,
        string methodName,
        IReadOnlyList<string> parameterTypes)
    {
        return $"{returnType} {declaringTypeDisplayName}::{methodName}({string.Join(",", parameterTypes)})";
    }

    public static string CreateInstantiatedTypeSubjectId(
        string genericTypeSubjectId,
        IReadOnlyList<string> typeArguments)
    {
        return $"{StripGenericArity(genericTypeSubjectId)}<{string.Join(",", typeArguments)}>";
    }

    public static string CreateInstantiatedTypeDisplayName(
        string genericTypeDisplayName,
        IReadOnlyList<string> typeArguments)
    {
        return $"{StripGenericArity(genericTypeDisplayName)}<{string.Join(",", typeArguments)}>";
    }

    public static string CreateGenericMethodName(string methodName, IReadOnlyList<string> genericArguments)
    {
        return $"{methodName}<{string.Join(",", genericArguments)}>";
    }

    public static string CreateMethodId(ManagedMethodModel method)
    {
        return string.Join(
            ".",
            [
                ToKebabCase(method.AssemblyName),
                ToKebabCase(method.DeclaringTypeDisplayName),
                ToKebabCase(method.Name),
            ]);
    }

    public static string CreateMethodSymbol(ManagedMethodModel method)
    {
        return string.Join(
            "_",
            [
                ToSymbolPart(method.AssemblyName),
                ToSymbolPart(method.DeclaringTypeDisplayName),
                ToSymbolPart(method.Name),
            ]);
    }

    public static string NormalizePathForManifest(string path, string baseDirectory)
    {
        var absolutePath = Path.GetFullPath(path);
        var absoluteBaseDirectory = Path.GetFullPath(baseDirectory);

        if (absolutePath.StartsWith(absoluteBaseDirectory, StringComparison.OrdinalIgnoreCase))
        {
            return Path.GetRelativePath(absoluteBaseDirectory, absolutePath).Replace('\\', '/');
        }

        return absolutePath.Replace('\\', '/');
    }

    public static string StripGenericArity(string value)
    {
        var builder = new System.Text.StringBuilder();

        for (var index = 0; index < value.Length; index++)
        {
            var current = value[index];
            if (current != '`')
            {
                builder.Append(current);
                continue;
            }

            index++;
            while (index < value.Length && char.IsDigit(value[index]))
            {
                index++;
            }

            index--;
        }

        return builder.ToString();
    }

    private static string GetTypeIdentityPart(string assemblyName, string? namespaceName, string typeName)
    {
        return string.Equals(namespaceName, assemblyName, StringComparison.Ordinal) || string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    private static string ToKebabCase(string value)
    {
        var builder = new System.Text.StringBuilder();

        for (var index = 0; index < value.Length; index++)
        {
            var current = value[index];

            if (char.IsLetterOrDigit(current))
            {
                if (char.IsUpper(current) && builder.Length > 0 && builder[^1] != '-')
                {
                    builder.Append('-');
                }

                builder.Append(char.ToLowerInvariant(current));
                continue;
            }

            if (builder.Length > 0 && builder[^1] != '-')
            {
                builder.Append('-');
            }
        }

        return builder.ToString().Trim('-');
    }

    private static string ToSymbolPart(string value)
    {
        if (string.Equals(value, ".ctor", StringComparison.Ordinal))
        {
            return "_ctor";
        }

        var builder = new System.Text.StringBuilder();

        foreach (var current in value)
        {
            builder.Append(char.IsLetterOrDigit(current) ? current : '_');
        }

        return builder.ToString().Trim('_');
    }
}

public sealed record AssemblyIdentityModel
{
    public required string Name { get; init; }

    public required Guid ModuleVersionId { get; init; }
}

public sealed record LoadedAssemblyModel
{
    public required string InputAssemblyPath { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }
}

public sealed record ManagedTypeModel
{
    public required string AssemblyName { get; init; }

    public required string? NamespaceName { get; init; }

    public required string Name { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DisplayName { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedFieldModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string FieldType { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedPropertyModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string PropertyType { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required int MetadataToken { get; init; }
}

public sealed record ManagedMethodModel
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DeclaringTypeDisplayName { get; init; }

    public required string Name { get; init; }

    public required string ReturnType { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string Signature { get; init; }

    public required bool IsStatic { get; init; }

    public required int MetadataToken { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }

    public ManagedImportModel? Import { get; init; }

    public required ManagedMethodBodyModel Body { get; init; }
}

public sealed record ManagedImportModel
{
    public required string ModuleName { get; init; }

    public required string EntryPointName { get; init; }
}

public sealed record ManagedParameterModel
{
    public required string Name { get; init; }

    public required string Type { get; init; }
}

public sealed record ManagedMethodBodyModel
{
    public required IReadOnlyList<ManagedBlockModel> Blocks { get; init; }
}

public sealed record ManagedBlockModel
{
    public required string BlockId { get; init; }

    public required IReadOnlyList<ManagedInstructionModel> Instructions { get; init; }
}

public sealed record ManagedInstructionModel
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedInstructionReference? Reference { get; init; }
}

public sealed record ManagedInstructionReference
{
    public required string AssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }
}

public sealed record CanonicalSubjectsModel
{
    public required IReadOnlyList<CanonicalSubjectModel> Subjects { get; init; }
}

public sealed record CanonicalSubjectModel
{
    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }

    public required string CanonicalSubjectId { get; init; }
}

public sealed record SemanticShapesModel
{
    public required IReadOnlyList<TypeShapeModel> Types { get; init; }

    public required IReadOnlyList<FieldShapeModel> Fields { get; init; }

    public required IReadOnlyList<PropertyShapeModel> Properties { get; init; }

    public required IReadOnlyList<MethodShapeModel> Methods { get; init; }
}

public sealed record TypeShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record FieldShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record PropertyShapeModel
{
    public required string SubjectId { get; init; }

    public required string Kind { get; init; }
}

public sealed record MethodShapeModel
{
    public required string SubjectId { get; init; }

    public required string MethodRole { get; init; }

    public required string BodyAvailability { get; init; }
}

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

    public required AssemblyIdentityModel Assembly { get; init; }

    public required string EntryPointSubjectId { get; init; }

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

    public required AssemblyIdentityModel Assembly { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }

    public required IReadOnlyList<LinkedDependencyModel> Dependencies { get; init; }

    public required CanonicalSubjectsModel CanonicalSubjects { get; init; }

    public required SemanticShapesModel SemanticShapes { get; init; }

    public required CapabilityBundlesModel CapabilityBundles { get; init; }

    public required OptimizationFactsArtifact OptimizationFacts { get; init; }
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
}

public sealed record TypedIlIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "typedIlIr";

    public required IReadOnlyList<TypedIlMethodArtifact> Methods { get; init; }
}

public sealed record TypedIlMethodArtifact
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public required string MethodRole { get; init; }

    public required string BodyAvailability { get; init; }

    public required IReadOnlyList<string> Capabilities { get; init; }

    public required IReadOnlyList<TypedIlParameterArtifact> Parameters { get; init; }

    public required IReadOnlyList<TypedIlBlockArtifact> Blocks { get; init; }
}

public sealed record TypedIlParameterArtifact
{
    public required string Name { get; init; }

    public required string Type { get; init; }
}

public sealed record TypedIlBlockArtifact
{
    public required string BlockId { get; init; }

    public required IReadOnlyList<TypedIlInstructionArtifact> Instructions { get; init; }
}

public sealed record TypedIlInstructionArtifact
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }
}

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

    public required string InputModuleVersionId { get; init; }

    public required IReadOnlyList<ManagedClosureArtifactRef> Artifacts { get; init; }
}

public sealed record ManagedClosureResult
{
    public required string OutputRootPath { get; init; }

    public required TypedIlIrArtifact TypedIlIr { get; init; }

    public required AotManifestArtifact AotManifest { get; init; }

    public required MetadataRegistrationArtifact MetadataRegistration { get; init; }

    public required CodeRegistrationArtifact CodeRegistration { get; init; }

    public required OptimizationFactsArtifact OptimizationFacts { get; init; }

    public required NativeReferenceLoweringPlanArtifact NativeReferenceLoweringPlan { get; init; }

    public required ManagedClosureManifestArtifact ClosureManifest { get; init; }
}

public sealed record NativeReferenceProofRequest(string ManagedClosureRootPath, string OutputRootPath);

public static class NativeReferenceArtifactNames
{
    public const string GeneratedDirectory = "generated";
    public const string GeneratedTranslationUnit = "generated/native-reference.generated.cpp";
    public const string LoweringPlan = "native-proof.plan.json";
    public const string ProofManifest = "native-proof.manifest.json";
}

public sealed record NativeReferenceGeneratedArtifactRef
{
    public required string Kind { get; init; }

    public required string Path { get; init; }
}

public sealed record NativeReferenceProofManifestArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeReferenceProofManifest";

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string ManagedClosureRootPath { get; init; }

    public required string PlanArtifactPath { get; init; }

    public required IReadOnlyList<NativeReferenceGeneratedArtifactRef> GeneratedArtifacts { get; init; }
}

public sealed record NativeReferenceLoweringPlanArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "nativeReferenceLoweringPlan";

    public required string PlanKind { get; init; }

    public required string AssemblyName { get; init; }

    public required string EntrySubjectId { get; init; }

    public required string IncludeHeader { get; init; }

    public required string ProofFunctionName { get; init; }

    public required string EntrySymbol { get; init; }

    public string? ConstructorSymbol { get; init; }

    public string? InstanceMethodSymbol { get; init; }

    public string? EchoMethodSymbol { get; init; }

    public string? GetterSymbol { get; init; }

    public required string ReferenceTypeToken { get; init; }

    public required string CapturedFieldToken { get; init; }

    public required string EntryMethodToken { get; init; }

    public string? ConstructorMethodToken { get; init; }

    public string? InstanceMethodToken { get; init; }

    public string? DispatchStrategy { get; init; }

    public string? EchoMethodToken { get; init; }

    public string? GetterMethodToken { get; init; }

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
}

public sealed record NativeReferenceGeneratedSource
{
    public required string RelativePath { get; init; }

    public required string Contents { get; init; }
}

public sealed record NativeReferenceProofResult
{
    public required string OutputRootPath { get; init; }

    public required NativeReferenceLoweringPlanArtifact LoweringPlan { get; init; }

    public required NativeReferenceProofManifestArtifact ProofManifest { get; init; }

    public required IReadOnlyList<NativeReferenceGeneratedSource> GeneratedSources { get; init; }
}
