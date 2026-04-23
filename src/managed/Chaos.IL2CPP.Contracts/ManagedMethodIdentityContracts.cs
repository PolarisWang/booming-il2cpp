namespace Chaos.IL2CPP.Contracts;

public sealed record ManagedMethodIdentityArtifact
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string SubjectId { get; init; }

    public required string MethodId { get; init; }

    public required string Signature { get; init; }

    public string? ExecutionAuthorityKey { get; init; }

    public InstantiationStubId? InstantiationStubId { get; init; }
}

public sealed record ManagedMethodIdentitySpec
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DeclaringTypeDisplayName { get; init; }

    public required string MethodName { get; init; }

    public required string SubjectId { get; init; }

    public required string Signature { get; init; }

    public string? DefinitionSubjectId { get; init; }

    public string? ExecutionAuthorityKey { get; init; }

    public InstantiationStubId? InstantiationStubId { get; init; }
}

public static class ManagedMethodIdentityResolver
{
    public static ManagedMethodIdentityArtifact Create(
        ManagedMethodModel method,
        string? executionAuthorityKey = null)
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
            ExecutionAuthorityKey = executionAuthorityKey,
            InstantiationStubId = method.RuntimeGenericContext?.InstantiationStubId,
        };
    }

    public static ManagedMethodIdentityArtifact Create(
        ManagedMethodIdentitySpec spec)
    {
        ArgumentNullException.ThrowIfNull(spec);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.AssemblyName);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.DeclaringTypeSubjectId);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.DeclaringTypeDisplayName);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.MethodName);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.SubjectId);
        ArgumentException.ThrowIfNullOrWhiteSpace(spec.Signature);

        var resolvedDefinitionSubjectId = string.IsNullOrWhiteSpace(spec.DefinitionSubjectId)
            ? spec.SubjectId
            : spec.DefinitionSubjectId;

        return new ManagedMethodIdentityArtifact
        {
            AssemblyName = spec.AssemblyName,
            DeclaringTypeSubjectId = spec.DeclaringTypeSubjectId,
            DefinitionSubjectId = resolvedDefinitionSubjectId,
            SubjectId = spec.SubjectId,
            MethodId = ManagedNaming.CreateMethodId(spec.AssemblyName, spec.DeclaringTypeDisplayName, spec.MethodName),
            Signature = spec.Signature,
            ExecutionAuthorityKey = spec.ExecutionAuthorityKey,
            InstantiationStubId = spec.InstantiationStubId,
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

    public static string ResolveExecutionAuthorityKey(ManagedMethodIdentityArtifact identity)
    {
        ArgumentNullException.ThrowIfNull(identity);

        if (!string.IsNullOrWhiteSpace(identity.ExecutionAuthorityKey))
        {
            return identity.ExecutionAuthorityKey;
        }

        if (!string.IsNullOrWhiteSpace(identity.InstantiationStubId?.Value))
        {
            return identity.InstantiationStubId.Value;
        }

        if (!string.IsNullOrWhiteSpace(identity.DefinitionSubjectId))
        {
            return identity.DefinitionSubjectId;
        }

        return ResolveSubjectId(identity);
    }

    public static string ResolveExecutionAuthorityKey(
        ManagedMethodIdentityArtifact? identity,
        string? fallbackSubjectId,
        string? fallbackDefinitionSubjectId = null)
    {
        if (identity is not null)
        {
            return ResolveExecutionAuthorityKey(identity);
        }

        if (!string.IsNullOrWhiteSpace(fallbackDefinitionSubjectId))
        {
            return fallbackDefinitionSubjectId;
        }

        if (!string.IsNullOrWhiteSpace(fallbackSubjectId))
        {
            return fallbackSubjectId;
        }

        throw new InvalidOperationException("managed method identity is missing execution authority.");
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

