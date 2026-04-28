namespace Chaos.IL2CPP.Contracts;

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

    public static string CreatePropertySubjectId(
        string declaringTypeSubjectId,
        string propertyName,
        IReadOnlyList<string>? indexParameterTypes = null)
    {
        var parameterSignature = indexParameterTypes is { Count: > 0 }
            ? $"[{string.Join(",", indexParameterTypes)}]"
            : string.Empty;
        return $"{declaringTypeSubjectId}::property:{propertyName}{parameterSignature}";
    }

    public static string CreateMethodSubjectId(
        string declaringTypeSubjectId,
        string methodName,
        string returnType,
        IReadOnlyList<string> parameterTypes,
        int genericParameterCount = 0)
    {
        return $"{declaringTypeSubjectId}::{CreateMethodIdentityName(methodName, genericParameterCount)}:{returnType}({string.Join(",", parameterTypes)})";
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

    public static string CreateMethodIdentityName(string methodName, int genericParameterCount = 0)
    {
        return genericParameterCount > 0
            ? $"{StripGenericArity(methodName)}`{genericParameterCount}"
            : methodName;
    }

    public static bool MatchesMethodSubjectId(string candidateSubjectId, string requestedSubjectId)
    {
        if (string.Equals(candidateSubjectId, requestedSubjectId, StringComparison.Ordinal))
        {
            return true;
        }

        if (!TryParseMethodSubjectIdComponents(candidateSubjectId, out var candidateDeclaringTypeSubjectId, out var candidateMethodName, out var candidateReturnType, out var candidateParameterSignature) ||
            !TryParseMethodSubjectIdComponents(requestedSubjectId, out var requestedDeclaringTypeSubjectId, out var requestedMethodName, out var requestedReturnType, out var requestedParameterSignature))
        {
            return false;
        }

        if (!string.Equals(candidateDeclaringTypeSubjectId, requestedDeclaringTypeSubjectId, StringComparison.Ordinal) ||
            !string.Equals(candidateMethodName, requestedMethodName, StringComparison.Ordinal) ||
            !string.Equals(candidateParameterSignature, requestedParameterSignature, StringComparison.Ordinal))
        {
            return false;
        }

        return string.IsNullOrWhiteSpace(candidateReturnType) ||
               string.IsNullOrWhiteSpace(requestedReturnType) ||
               string.Equals(candidateReturnType, requestedReturnType, StringComparison.Ordinal);
    }

    public static string GetMethodSubjectIdDisplayString(string subjectId)
    {
        if (!TryParseMethodSubjectIdComponents(subjectId, out var declaringTypeSubjectId, out var methodName, out _, out var parameterSignature))
        {
            return subjectId;
        }

        return $"{declaringTypeSubjectId}::{methodName}({parameterSignature})";
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

    public static string CreateInstantiationStubSymbol(InstantiationStubId instantiationStubId)
    {
        ArgumentNullException.ThrowIfNull(instantiationStubId);

        return $"chaos_{ToSymbolPart(instantiationStubId.Value)}";
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

    public static string CanonicalizeSubjectId(string subjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return CanonicalizeGenericPlaceholderOrdinals(subjectId);
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

    public static GenericInstantiationKey? TryCreateGenericInstantiationKey(string subjectId, string definitionSubjectId)
    {
        return TryParseGenericInstantiation(subjectId, definitionSubjectId, out var parsed)
            ? parsed.InstantiationKey
            : null;
    }

    public static GenericDiagnosticArtifact? TryCreateGenericDiagnosticArtifact(string subjectId, string definitionSubjectId)
    {
        if (!TryParseGenericInstantiation(subjectId, definitionSubjectId, out var parsed))
        {
            return null;
        }

        return new GenericDiagnosticArtifact
        {
            SubjectId = subjectId,
            DefinitionSubjectId = definitionSubjectId,
            DisplaySubjectId = parsed.DisplaySubjectId,
            InstantiationKey = parsed.InstantiationKey,
        };
    }

    public static SharedGenericBodyId CreateSharedGenericBodyId(GenericInstantiationKey instantiationKey)
    {
        ArgumentNullException.ThrowIfNull(instantiationKey);

        return new SharedGenericBodyId
        {
            Value = $"body:{CreateGenericKernelIdentityValue(instantiationKey)}",
        };
    }

    public static InstantiationStubId CreateInstantiationStubId(GenericInstantiationKey instantiationKey)
    {
        ArgumentNullException.ThrowIfNull(instantiationKey);

        return new InstantiationStubId
        {
            Value = $"stub:{CreateGenericKernelIdentityValue(instantiationKey)}",
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

    private static bool TryParseMethodSubjectIdComponents(
        string subjectId,
        out string declaringTypeSubjectId,
        out string methodName,
        out string? returnType,
        out string parameterSignature)
    {
        declaringTypeSubjectId = string.Empty;
        methodName = string.Empty;
        returnType = null;
        parameterSignature = string.Empty;

        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var parameterListIndex = subjectId.LastIndexOf('(');
        if (separatorIndex <= 0 ||
            parameterListIndex <= separatorIndex + 2 ||
            !subjectId.EndsWith(")", StringComparison.Ordinal))
        {
            return false;
        }

        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':', parameterListIndex);
        var hasReturnType = returnTypeSeparatorIndex > separatorIndex + 1;
        var methodNameEndIndex = hasReturnType
            ? returnTypeSeparatorIndex
            : parameterListIndex;
        if (methodNameEndIndex <= separatorIndex + 2)
        {
            return false;
        }

        declaringTypeSubjectId = subjectId[..separatorIndex];
        methodName = subjectId[(separatorIndex + 2)..methodNameEndIndex];
        if (hasReturnType)
        {
            returnType = subjectId[(returnTypeSeparatorIndex + 1)..parameterListIndex];
        }

        parameterSignature = subjectId[(parameterListIndex + 1)..^1];
        return true;
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

    private static bool TryParseGenericInstantiation(
        string subjectId,
        string definitionSubjectId,
        out ParsedGenericInstantiation parsed)
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
            parsed = default;
            return false;
        }

        parsed = new ParsedGenericInstantiation(
            new GenericInstantiationKey
            {
                ContextKind = hasTypeArguments && hasMethodArguments
                    ? GenericContextKind.TypeAndMethodInstantiation
                    : hasTypeArguments
                        ? GenericContextKind.TypeInstantiation
                        : GenericContextKind.MethodInstantiation,
                DefinitionSubjectId = definitionSubjectId,
                TypeArguments = typeArguments,
                MethodArguments = methodArguments,
            },
            LooksLikeMethodSubjectId(subjectId)
                ? GetMethodSubjectIdDisplayString(subjectId)
                : subjectId);
        return true;
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
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':', parameterListIndex >= 0 ? parameterListIndex : subjectId.Length - 1);
        if (separatorIndex <= 0 ||
            parameterListIndex <= separatorIndex + 2 ||
            returnTypeSeparatorIndex <= separatorIndex + 2)
        {
            throw new InvalidOperationException($"subject id '{subjectId}' is missing method name.");
        }

        return subjectId[(separatorIndex + 2)..returnTypeSeparatorIndex];
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

    private static string CreateGenericKernelIdentityValue(GenericInstantiationKey instantiationKey)
    {
        return $"definition={instantiationKey.DefinitionSubjectId};type={FormatGenericArgumentList(instantiationKey.TypeArguments)};method={FormatGenericArgumentList(instantiationKey.MethodArguments)}";
    }

    private static string FormatGenericArgumentList(IReadOnlyList<string>? arguments)
    {
        if (arguments is not { Count: > 0 })
        {
            return "[]";
        }

        return $"[{string.Join(",", arguments)}]";
    }

    private static string CanonicalizeGenericPlaceholderOrdinals(string subjectId)
    {
        var placeholderMaps = new Dictionary<int, IReadOnlyDictionary<string, string>>();
        foreach (var placeholderPrefixLength in GetGenericPlaceholderPrefixLengths(subjectId))
        {
            var placeholderMap = BuildGenericPlaceholderMap(subjectId, placeholderPrefixLength);
            if (placeholderMap.Count > 0)
            {
                placeholderMaps[placeholderPrefixLength] = placeholderMap;
            }
        }

        if (placeholderMaps.Count == 0)
        {
            return subjectId;
        }

        var builder = new System.Text.StringBuilder(subjectId.Length);
        for (var index = 0; index < subjectId.Length;)
        {
            if (subjectId[index] != '!')
            {
                builder.Append(subjectId[index]);
                index++;
                continue;
            }

            var bangStart = index;
            while (index < subjectId.Length && subjectId[index] == '!')
            {
                index++;
            }

            var digitStart = index;
            while (index < subjectId.Length && char.IsDigit(subjectId[index]))
            {
                index++;
            }

            var placeholderPrefixLength = digitStart - bangStart;
            if (digitStart > bangStart &&
                index > digitStart &&
                placeholderMaps.TryGetValue(placeholderPrefixLength, out var placeholderMap))
            {
                var placeholderToken = subjectId[bangStart..index];
                if (placeholderMap.TryGetValue(placeholderToken, out var canonicalPlaceholderToken))
                {
                    builder.Append(canonicalPlaceholderToken);
                    continue;
                }
            }

            builder.Append(subjectId.AsSpan(bangStart, index - bangStart));
        }

        return builder.ToString();
    }

    private static IReadOnlyList<int> GetGenericPlaceholderPrefixLengths(string subjectId)
    {
        var placeholderPrefixLengths = new SortedSet<int>();
        for (var index = 0; index < subjectId.Length;)
        {
            if (subjectId[index] != '!')
            {
                index++;
                continue;
            }

            var bangStart = index;
            while (index < subjectId.Length && subjectId[index] == '!')
            {
                index++;
            }

            var digitStart = index;
            while (index < subjectId.Length && char.IsDigit(subjectId[index]))
            {
                index++;
            }

            if (digitStart > bangStart && index > digitStart)
            {
                placeholderPrefixLengths.Add(digitStart - bangStart);
            }
        }

        return placeholderPrefixLengths.ToList();
    }

    private static IReadOnlyDictionary<string, string> BuildGenericPlaceholderMap(
        string subjectId,
        int placeholderPrefixLength)
    {
        var placeholderOrdinals = new SortedSet<int>();
        for (var index = 0; index < subjectId.Length;)
        {
            if (subjectId[index] != '!')
            {
                index++;
                continue;
            }

            var bangStart = index;
            while (index < subjectId.Length && subjectId[index] == '!')
            {
                index++;
            }

            var digitStart = index;
            while (index < subjectId.Length && char.IsDigit(subjectId[index]))
            {
                index++;
            }

            if (digitStart == bangStart ||
                index == digitStart ||
                digitStart - bangStart != placeholderPrefixLength ||
                !int.TryParse(subjectId.AsSpan(digitStart, index - digitStart), out var placeholderOrdinal))
            {
                continue;
            }

            placeholderOrdinals.Add(placeholderOrdinal);
        }

        if (placeholderOrdinals.Count == 0)
        {
            return new Dictionary<string, string>(StringComparer.Ordinal);
        }

        var placeholderPrefix = new string('!', placeholderPrefixLength);
        var placeholderMap = new Dictionary<string, string>(placeholderOrdinals.Count, StringComparer.Ordinal);
        var canonicalOrdinal = 0;
        foreach (var placeholderOrdinal in placeholderOrdinals)
        {
            placeholderMap[$"{placeholderPrefix}{placeholderOrdinal}"] =
                $"{placeholderPrefix}{canonicalOrdinal}";
            canonicalOrdinal++;
        }

        return placeholderMap;
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

    private readonly record struct ParsedGenericInstantiation(
        GenericInstantiationKey InstantiationKey,
        string DisplaySubjectId);
}
