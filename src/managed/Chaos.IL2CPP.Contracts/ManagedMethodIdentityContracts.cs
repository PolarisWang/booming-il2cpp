namespace Chaos.IL2CPP.Contracts;

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

