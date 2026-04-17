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
