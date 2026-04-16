namespace Chaos.IL2CPP.Contracts;

public sealed record ManagedClosureRequest(
    string InputAssemblyPath,
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
    public const string CodeRegistration = "code-registration.json";
    public const string OptimizationFacts = "optimization-facts.json";
    public const string PreserveDescriptor = "preserve-descriptor.json";
    public const string NativeReferenceLoweringPlan = "native-reference.lowering-plan.json";
    public const string NativeAotLoweringPlan = "native-aot.lowering-plan.json";
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
        ArgumentNullException.ThrowIfNull(method);
        return CreateMethodId(method.AssemblyName, method.DeclaringTypeDisplayName, method.Name);
    }

    public static string CreateMethodId(
        string assemblyName,
        string declaringTypeDisplayName,
        string methodName)
    {
        return string.Join(
            ".",
            [
                ToKebabCase(assemblyName),
                ToKebabCase(declaringTypeDisplayName),
                ToKebabCase(methodName),
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

    public static GenericContextArtifact? TryCreateGenericContext(string subjectId, string definitionSubjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentException.ThrowIfNullOrWhiteSpace(definitionSubjectId);

        IReadOnlyList<string> typeArguments;
        IReadOnlyList<string> methodArguments;

        if (subjectId.Contains("::", StringComparison.Ordinal) &&
            definitionSubjectId.Contains("::", StringComparison.Ordinal))
        {
            var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
            var definitionDeclaringTypeSubjectId = GetDeclaringTypeSubjectId(definitionSubjectId);
            typeArguments = TryExtractTypeArguments(declaringTypeSubjectId, definitionDeclaringTypeSubjectId);
            methodArguments = LooksLikeMethodSubjectId(subjectId) && LooksLikeMethodSubjectId(definitionSubjectId)
                ? TryExtractMethodArguments(subjectId, definitionSubjectId)
                : [];
        }
        else
        {
            typeArguments = TryExtractTypeArguments(subjectId, definitionSubjectId);
            methodArguments = [];
        }

        var hasTypeArguments = typeArguments.Count > 0;
        var hasMethodArguments = methodArguments.Count > 0;
        if (!hasTypeArguments && !hasMethodArguments)
        {
            return null;
        }

        return new GenericContextArtifact
        {
            ContextKind = hasTypeArguments && hasMethodArguments
                ? GenericContextKind.TypeAndMethodInstantiation
                : hasTypeArguments
                    ? GenericContextKind.TypeInstantiation
                    : GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = definitionSubjectId,
            TypeArguments = typeArguments,
            MethodArguments = methodArguments,
        };
    }

    private static string GetTypeIdentityPart(string assemblyName, string? namespaceName, string typeName)
    {
        return string.Equals(namespaceName, assemblyName, StringComparison.Ordinal) || string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    private static bool LooksLikeMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("::", StringComparison.Ordinal) &&
               subjectId.EndsWith(")", StringComparison.Ordinal);
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"subject id '{subjectId}' is missing declaring type information.");
        }

        return subjectId[..separatorIndex];
    }

    private static IReadOnlyList<string> TryExtractTypeArguments(string subjectId, string definitionSubjectId)
    {
        var strippedDefinition = StripGenericArity(definitionSubjectId);
        if (string.Equals(subjectId, definitionSubjectId, StringComparison.Ordinal) ||
            string.Equals(subjectId, strippedDefinition, StringComparison.Ordinal) ||
            !subjectId.StartsWith(strippedDefinition, StringComparison.Ordinal) ||
            subjectId.Length <= strippedDefinition.Length + 1 ||
            subjectId[strippedDefinition.Length] != '<' ||
            subjectId[^1] != '>')
        {
            return [];
        }

        return SplitTopLevelArguments(subjectId[(strippedDefinition.Length + 1)..^1]);
    }

    private static IReadOnlyList<string> TryExtractMethodArguments(string subjectId, string definitionSubjectId)
    {
        var methodName = GetMethodName(subjectId);
        var definitionMethodName = GetMethodName(definitionSubjectId);
        var strippedDefinitionMethodName = StripGenericArity(definitionMethodName);

        if (string.Equals(methodName, definitionMethodName, StringComparison.Ordinal) ||
            string.Equals(methodName, strippedDefinitionMethodName, StringComparison.Ordinal) ||
            !methodName.StartsWith(strippedDefinitionMethodName, StringComparison.Ordinal) ||
            methodName.Length <= strippedDefinitionMethodName.Length + 1 ||
            methodName[strippedDefinitionMethodName.Length] != '<' ||
            methodName[^1] != '>')
        {
            return [];
        }

        return SplitTopLevelArguments(methodName[(strippedDefinitionMethodName.Length + 1)..^1]);
    }

    private static string GetMethodName(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var parameterListIndex = subjectId.LastIndexOf('(');
        if (separatorIndex <= 0 || parameterListIndex <= separatorIndex + 2)
        {
            throw new InvalidOperationException($"subject id '{subjectId}' is missing method name.");
        }

        return subjectId[(separatorIndex + 2)..parameterListIndex];
    }

    private static IReadOnlyList<string> SplitTopLevelArguments(string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return [];
        }

        var arguments = new List<string>();
        var builder = new System.Text.StringBuilder();
        var genericDepth = 0;

        foreach (var character in value)
        {
            switch (character)
            {
                case '<':
                case '[':
                    genericDepth++;
                    builder.Append(character);
                    break;
                case '>':
                case ']':
                    genericDepth--;
                    builder.Append(character);
                    break;
                case ',' when genericDepth == 0:
                    arguments.Add(builder.ToString().Trim());
                    builder.Clear();
                    break;
                default:
                    builder.Append(character);
                    break;
            }
        }

        if (builder.Length > 0)
        {
            arguments.Add(builder.ToString().Trim());
        }

        return arguments;
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

    public bool IsInterface { get; init; }

    public bool IsValueType { get; init; }

    public string? BaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }

    public bool IsPreserved { get; init; }

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

    public bool IsStatic { get; init; }

    public bool IsThreadStatic { get; init; }

    public bool IsPreserved { get; init; }

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

    public bool IsPreserved { get; init; }

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

    public required bool IsVirtual { get; init; }

    public bool IsPreserved { get; init; }

    public bool IsUnmanagedCallersOnly { get; init; }

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

    public required IReadOnlyList<ManagedExceptionRegionModel> ExceptionRegions { get; init; }
}

public sealed record ManagedBlockModel
{
    public required string BlockId { get; init; }

    public required IReadOnlyList<ManagedInstructionModel> Instructions { get; init; }
}

public sealed record ManagedExceptionRegionModel
{
    public required string HandlingKind { get; init; }

    public required int TryOffset { get; init; }

    public required int TryLength { get; init; }

    public required int HandlerOffset { get; init; }

    public required int HandlerLength { get; init; }

    public int? FilterOffset { get; init; }

    public string? CatchTypeSubjectId { get; init; }
}

public sealed record ManagedInstructionModel
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public int? IlOffset { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }
}

public sealed record ManagedInstructionReference
{
    public required string AssemblyName { get; init; }

    public required string SubjectKind { get; init; }

    public required string SubjectId { get; init; }
}

/// <summary>
/// Canonical call-site kinds carried by managed / typed-il / AOT core instruction artifacts.
/// </summary>
public enum ManagedCallSiteKind : byte
{
    /// <summary>
    /// Call site describes an indirect call through a function pointer.
    /// </summary>
    FunctionPointer = 1,
}

/// <summary>
/// Stable call-site signature metadata needed by indirect-call instructions such as <c>calli</c>.
/// </summary>
public sealed record ManagedCallSiteSignature
{
    public required ManagedCallSiteKind KindCode { get; init; }

    public required string ReturnType { get; init; }

    public required IReadOnlyList<string> ParameterTypes { get; init; }
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

    public required BodyAvailabilityCode BodyAvailabilityCode { get; init; }
}

public enum BodyAvailabilityCode : byte
{
    None = 0,
    NativeGenerated = 1,
    InterpreterReady = 2,
    BridgeDispatch = 3,
    MetadataOnly = 4,
    ExternalRuntime = 5,
    Unsupported = 6,
}

public enum HybridDispatchKind : byte
{
    None = 0,
    Direct = 1,
    Virtual = 2,
    Bridge = 3,
    ExternalRuntime = 4,
    Unsupported = 5,
}

/// <summary>
/// Encodes whether a generic context is carried by a closed type, a closed method, or both.
/// </summary>
public enum GenericContextKind : byte
{
    /// <summary>
    /// Closed context comes from the declaring type instantiation.
    /// </summary>
    TypeInstantiation = 1,

    /// <summary>
    /// Closed context comes from the method instantiation.
    /// </summary>
    MethodInstantiation = 2,

    /// <summary>
    /// Closed context carries both declaring-type and method instantiation arguments.
    /// </summary>
    TypeAndMethodInstantiation = 3,
}

public sealed record GenericContextArtifact
{
    public required GenericContextKind ContextKind { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public IReadOnlyList<string>? TypeArguments { get; init; }

    public IReadOnlyList<string>? MethodArguments { get; init; }
}

public sealed record ManagedMethodIdentityArtifact
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string SubjectId { get; init; }

    public required string MethodId { get; init; }

    public required string Signature { get; init; }
}

public static class ManagedMethodIdentityResolver
{
    public static ManagedMethodIdentityArtifact Create(ManagedMethodModel method)
    {
        ArgumentNullException.ThrowIfNull(method);

        return new ManagedMethodIdentityArtifact
        {
            AssemblyName = method.AssemblyName,
            DeclaringTypeSubjectId = method.DeclaringTypeSubjectId,
            DefinitionSubjectId = method.DefinitionSubjectId,
            SubjectId = method.SubjectId,
            MethodId = ManagedNaming.CreateMethodId(method),
            Signature = method.Signature,
        };
    }

    public static ManagedMethodIdentityArtifact Create(string subjectId, string signature, string? definitionSubjectId = null)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentException.ThrowIfNullOrWhiteSpace(signature);

        var assemblyName = ResolveAssemblyName(subjectId);
        var declaringTypeSubjectId = ResolveDeclaringTypeSubjectId(subjectId);
        var declaringTypeDisplayName = ResolveDeclaringTypeDisplayName(declaringTypeSubjectId);
        var methodName = ResolveMethodName(subjectId);

        return new ManagedMethodIdentityArtifact
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = declaringTypeSubjectId,
            DefinitionSubjectId = string.IsNullOrWhiteSpace(definitionSubjectId)
                ? subjectId
                : definitionSubjectId,
            SubjectId = subjectId,
            MethodId = ManagedNaming.CreateMethodId(assemblyName, declaringTypeDisplayName, methodName),
            Signature = signature,
        };
    }

    public static string ResolveSubjectId(ManagedMethodIdentityArtifact identity)
    {
        ArgumentNullException.ThrowIfNull(identity);
        ArgumentException.ThrowIfNullOrWhiteSpace(identity.SubjectId);
        return identity.SubjectId;
    }

    public static string ResolveSubjectId(ManagedMethodIdentityArtifact? identity, string? fallbackSubjectId)
    {
        if (identity is not null)
        {
            return ResolveSubjectId(identity);
        }

        if (!string.IsNullOrWhiteSpace(fallbackSubjectId))
        {
            return fallbackSubjectId;
        }

        throw new InvalidOperationException("managed method identity is missing subject id.");
    }

    public static int ResolveParameterCount(ManagedMethodIdentityArtifact? identity, string? fallbackSubjectId = null)
    {
        if (identity is not null && !string.IsNullOrWhiteSpace(identity.Signature))
        {
            return CountParameters(identity.Signature);
        }

        return CountParameters(ResolveSubjectId(identity, fallbackSubjectId));
    }

    private static int CountParameters(string value)
    {
        var openParenthesis = value.LastIndexOf('(');
        var closeParenthesis = value.LastIndexOf(')');
        if (openParenthesis < 0 || closeParenthesis < openParenthesis)
        {
            throw new InvalidOperationException($"method identity '{value}' is missing parameter list.");
        }

        var parameterList = value[(openParenthesis + 1)..closeParenthesis];
        if (string.IsNullOrWhiteSpace(parameterList))
        {
            return 0;
        }

        var parameterCount = 1;
        var genericDepth = 0;
        foreach (var character in parameterList)
        {
            switch (character)
            {
                case '<':
                case '[':
                    genericDepth++;
                    break;
                case '>':
                case ']':
                    genericDepth--;
                    break;
                case ',' when genericDepth == 0:
                    parameterCount++;
                    break;
            }
        }

        return parameterCount;
    }

    private static string ResolveAssemblyName(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf('/', StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"method identity '{subjectId}' is missing assembly prefix.");
        }

        return subjectId[..separatorIndex];
    }

    private static string ResolveDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"method identity '{subjectId}' is missing declaring type.");
        }

        return subjectId[..separatorIndex];
    }

    private static string ResolveDeclaringTypeDisplayName(string declaringTypeSubjectId)
    {
        var separatorIndex = declaringTypeSubjectId.IndexOf('/', StringComparison.Ordinal);
        if (separatorIndex <= 0 || separatorIndex == declaringTypeSubjectId.Length - 1)
        {
            throw new InvalidOperationException(
                $"declaring type subject '{declaringTypeSubjectId}' is missing display name.");
        }

        return declaringTypeSubjectId[(separatorIndex + 1)..];
    }

    private static string ResolveMethodName(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var parameterListIndex = subjectId.LastIndexOf('(');
        if (separatorIndex <= 0 || parameterListIndex <= separatorIndex + 2)
        {
            throw new InvalidOperationException($"method identity '{subjectId}' is missing method name.");
        }

        return subjectId[(separatorIndex + 2)..parameterListIndex];
    }
}

public static class BodyAvailabilityResolver
{
    public static BodyAvailabilityCode Resolve(ManagedMethodModel method)
    {
        ArgumentNullException.ThrowIfNull(method);

        if (method.Import is not null)
        {
            return BodyAvailabilityCode.ExternalRuntime;
        }

        return method.Body.Blocks.Any(block => block.Instructions.Count > 0)
            ? BodyAvailabilityCode.InterpreterReady
            : BodyAvailabilityCode.Unsupported;
    }

    public static string ToLegacyLabel(BodyAvailabilityCode bodyAvailabilityCode)
    {
        return bodyAvailabilityCode switch
        {
            BodyAvailabilityCode.NativeGenerated or BodyAvailabilityCode.InterpreterReady => "has-canonical-body",
            _ => "no-canonical-body",
        };
    }
}

public static class HybridDispatchResolver
{
    public static HybridDispatchKind? ResolveInstruction(
        string callerAssemblyName,
        IReadOnlySet<string> internalAssemblyNames,
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, ManagedMethodModel>? methodsBySubjectId = null)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(callerAssemblyName);
        ArgumentNullException.ThrowIfNull(internalAssemblyNames);
        ArgumentNullException.ThrowIfNull(instruction);

        if (!string.Equals(instruction.Op, "call", StringComparison.Ordinal) &&
            !string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return null;
        }

        var calleeAssemblyName = instruction.Reference?.AssemblyName ?? TryGetAssemblyNameFromSubjectId(instruction.Callee);
        if (string.IsNullOrWhiteSpace(calleeAssemblyName))
        {
            return HybridDispatchKind.Unsupported;
        }

        if (!internalAssemblyNames.Contains(calleeAssemblyName))
        {
            return HybridDispatchKind.ExternalRuntime;
        }

        if (!string.Equals(callerAssemblyName, calleeAssemblyName, StringComparison.Ordinal))
        {
            return HybridDispatchKind.Bridge;
        }

        if (!string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return HybridDispatchKind.Direct;
        }

        var calleeSubjectId = instruction.Callee ?? instruction.Reference?.SubjectId;
        if (!string.IsNullOrWhiteSpace(calleeSubjectId) &&
            methodsBySubjectId is not null &&
            methodsBySubjectId.TryGetValue(calleeSubjectId, out var calleeMethod))
        {
            return calleeMethod.IsVirtual
                ? HybridDispatchKind.Virtual
                : HybridDispatchKind.Direct;
        }

        return HybridDispatchKind.Virtual;
    }

    private static string? TryGetAssemblyNameFromSubjectId(string? subjectId)
    {
        if (string.IsNullOrWhiteSpace(subjectId))
        {
            return null;
        }

        var separatorIndex = subjectId.IndexOf('/', StringComparison.Ordinal);
        return separatorIndex <= 0 ? null : subjectId[..separatorIndex];
    }
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

    public required IReadOnlyList<AssemblyIdentityModel> Assemblies { get; init; }

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

    public required IReadOnlyList<AssemblyIdentityModel> Assemblies { get; init; }

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

public sealed record TypedIlIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "typedIlIr";

    public required IReadOnlyList<TypedIlMethodArtifact> Methods { get; init; }
}

/// <summary>
/// Canonical reference kinds carried by <see cref="AotCoreIrArtifact"/>.
/// </summary>
public enum AotCoreIrReferenceKind : byte
{
    /// <summary>
    /// Reference points to a managed type subject.
    /// </summary>
    Type = 1,

    /// <summary>
    /// Reference points to a managed field subject.
    /// </summary>
    Field = 2,

    /// <summary>
    /// Reference points to a managed method subject.
    /// </summary>
    Method = 3,
}

/// <summary>
/// Minimal type-shape information carried by <see cref="AotCoreIrReferenceArtifact"/>.
/// </summary>
public enum AotCoreIrTypeShapeKind : byte
{
    /// <summary>
    /// Type is a reference type.
    /// </summary>
    ReferenceType = 1,

    /// <summary>
    /// Type is a value type.
    /// </summary>
    ValueType = 2,

    /// <summary>
    /// Type is an interface.
    /// </summary>
    InterfaceType = 3,
}

/// <summary>
/// Minimal object/runtime services required by the current AOT core IR slice.
/// </summary>
public enum AotCoreIrRuntimeServiceKind : byte
{
    /// <summary>
    /// Allocates an object instance and wires constructor dispatch.
    /// </summary>
    NewObject = 1,

    /// <summary>
    /// Loads an instance field value.
    /// </summary>
    LoadInstanceField = 2,

    /// <summary>
    /// Stores an instance field value.
    /// </summary>
    StoreInstanceField = 3,

    /// <summary>
    /// Loads a static field value.
    /// </summary>
    LoadStaticField = 4,

    /// <summary>
    /// Stores a static field value.
    /// </summary>
    StoreStaticField = 5,

    /// <summary>
    /// Allocates a managed array.
    /// </summary>
    NewArray = 6,

    /// <summary>
    /// Performs a checked reference cast.
    /// </summary>
    CastClass = 7,

    /// <summary>
    /// Performs an instance-of compatible cast probe.
    /// </summary>
    IsInst = 8,

    /// <summary>
    /// Boxes a value type into a managed object.
    /// </summary>
    Box = 9,

    /// <summary>
    /// Produces a managed pointer to boxed storage.
    /// </summary>
    Unbox = 10,

    /// <summary>
    /// Extracts a value from boxed storage.
    /// </summary>
    UnboxAny = 11,

    /// <summary>
    /// Loads a managed array element.
    /// </summary>
    LoadArrayElement = 12,

    /// <summary>
    /// Stores a managed array element.
    /// </summary>
    StoreArrayElement = 13,

    /// <summary>
    /// Initializes value-type storage through a managed pointer.
    /// </summary>
    InitObject = 14,
}

/// <summary>
/// Encodes managed EH region shape after lowering into AOT Core IR.
/// </summary>
public enum AotCoreIrExceptionRegionKind : byte
{
    /// <summary>
    /// Typed catch handler.
    /// </summary>
    Catch = 1,

    /// <summary>
    /// Finally handler.
    /// </summary>
    Finally = 2,

    /// <summary>
    /// Fault handler.
    /// </summary>
    Fault = 3,

    /// <summary>
    /// Filtered catch handler.
    /// </summary>
    Filter = 4,
}

/// <summary>
/// Encodes the lowered ABI carrier used by Native AOT direct-call signatures.
/// </summary>
public enum AotCoreIrAbiCarrierKind : byte
{
    /// <summary>
    /// Method returns no value.
    /// </summary>
    Void = 0,

    /// <summary>
    /// 32-bit integer carrier.
    /// </summary>
    Int32 = 1,

    /// <summary>
    /// Native pointer-sized integer carrier.
    /// </summary>
    NativeInt = 2,

    /// <summary>
    /// Value type travels by value through the native ABI.
    /// </summary>
    ValueTypeByValue = 3,

    /// <summary>
    /// Signed 8-bit integer carrier.
    /// </summary>
    Int8 = 4,

    /// <summary>
    /// Unsigned 8-bit integer carrier.
    /// </summary>
    UInt8 = 5,

    /// <summary>
    /// Signed 16-bit integer carrier.
    /// </summary>
    Int16 = 6,

    /// <summary>
    /// Unsigned 16-bit integer carrier.
    /// </summary>
    UInt16 = 7,

    /// <summary>
    /// 32-bit floating-point carrier.
    /// </summary>
    Float32 = 8,

    /// <summary>
    /// 64-bit floating-point carrier.
    /// </summary>
    Float64 = 9,

    /// <summary>
    /// Signed 64-bit integer carrier.
    /// </summary>
    Int64 = 10,

    /// <summary>
    /// Unsigned 64-bit integer carrier.
    /// </summary>
    UInt64 = 11,
}

/// <summary>
/// Minimal ABI slot information consumed by Native AOT lowering.
/// </summary>
public sealed record AotCoreIrAbiSlotArtifact
{
    public required AotCoreIrAbiCarrierKind CarrierKindCode { get; init; }

    public string? TypeSubjectId { get; init; }

    public AotCoreIrTypeShapeKind TypeShape { get; init; }
}

public sealed record AotCoreIrReferenceArtifact
{
    public required AotCoreIrReferenceKind Kind { get; init; }

    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public GenericContextArtifact? GenericContext { get; init; }

    public AotCoreIrTypeShapeKind TypeShape { get; init; }

    public string? ArrayElementSubjectId { get; init; }

    public AotCoreIrTypeShapeKind ArrayElementTypeShape { get; init; }

    public string? ArrayElementBaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ArrayElementImplementedInterfaceSubjectIds { get; init; }

    public string? BaseTypeSubjectId { get; init; }

    public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }

    public string? DeclaringTypeSubjectId { get; init; }

    public AotCoreIrTypeShapeKind DeclaringTypeShape { get; init; }
}

public sealed record AotCoreIrArtifact
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "aotCoreIr";

    public required IReadOnlyList<AotCoreIrMethodArtifact> Methods { get; init; }
}

public sealed record AotCoreIrMethodArtifact
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public GenericContextArtifact? GenericContext { get; init; }

    public required string NativeSymbol { get; init; }

    public required bool IsStatic { get; init; }

    public required string ReturnType { get; init; }

    public required AotCoreIrAbiSlotArtifact ReturnAbi { get; init; }

    public required int ParameterCount { get; init; }

    public required IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis { get; init; }

    public required int LocalCount { get; init; }

    public required int ExceptionRegionCount { get; init; }

    public required IReadOnlyList<AotCoreIrExceptionRegionArtifact> ExceptionRegions { get; init; }

    public required IReadOnlyList<AotCoreIrInstructionArtifact> Instructions { get; init; }
}

public sealed record AotCoreIrExceptionRegionArtifact
{
    public required AotCoreIrExceptionRegionKind HandlingKindCode { get; init; }

    public required int TryOffset { get; init; }

    public required int TryLength { get; init; }

    public required int HandlerOffset { get; init; }

    public required int HandlerLength { get; init; }

    public int? FilterOffset { get; init; }

    public string? CatchTypeSubjectId { get; init; }
}

public sealed record AotCoreIrInstructionArtifact
{
    public required string Op { get; init; }

    public object? Operand { get; init; }

    public required int IlOffset { get; init; }

    public string? ResultType { get; init; }

    public string? Callee { get; init; }

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }

    public AotCoreIrReferenceArtifact? TargetReference { get; init; }

    public AotCoreIrRuntimeServiceKind? RuntimeServiceKind { get; init; }

    public string? TargetSymbol { get; init; }

    public int? TargetParameterCount { get; init; }

    public string? TargetReturnType { get; init; }

    public HybridDispatchKind? DispatchKindCode { get; init; }
}

public sealed record TypedIlMethodArtifact
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public required ManagedMethodIdentityArtifact Identity { get; init; }

    public required string MethodRole { get; init; }

    public required string BodyAvailability { get; init; }

    public required BodyAvailabilityCode BodyAvailabilityCode { get; init; }

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

    public ManagedCallSiteSignature? CallSiteSignature { get; init; }

    public ManagedInstructionReference? Reference { get; init; }

    public HybridDispatchKind? DispatchKindCode { get; init; }
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
