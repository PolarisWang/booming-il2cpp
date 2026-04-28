using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{

    private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(
        AotCoreIrArtifact aotCoreIr,
        AotCoreIrMethodArtifact entryMethod)
    {
        var ordered = new List<AotCoreIrMethodArtifact>();
        var visited = new HashSet<string>(StringComparer.Ordinal);

        void Visit(AotCoreIrMethodArtifact method)
        {
            if (!visited.Add(method.SubjectId))
            {
                return;
            }

            foreach (var instruction in method.Instructions)
            {
                foreach (var reachableMethod in ResolveReachableMethods(instruction))
                {
                    Visit(reachableMethod);
                }
            }

            ordered.Add(method);
        }

        Visit(entryMethod);
        return ordered;
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(
        AotCoreIrInstructionArtifact instruction)
    {
        if (string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return instruction.DispatchKindCode switch
            {
                HybridDispatchKind.Virtual => ResolveVirtualDispatchTargets(instruction),
                HybridDispatchKind.ExternalRuntime => ResolveExternalRuntimeReachableMethods(instruction),
                _ => ResolveDirectReachableMethods(instruction),
            };
        }

        if (string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal) ||
            string.Equals(instruction.Op, "ldvirtftn", StringComparison.Ordinal))
        {
            return ResolveDirectReachableMethods(instruction);
        }

        if (string.Equals(instruction.Op, "newobj", StringComparison.Ordinal))
        {
            return ResolveDirectReachableMethods(instruction);
        }

        if (!string.Equals(instruction.Op, "call", StringComparison.Ordinal))
        {
            return [];
        }

        return ResolveDirectReachableMethods(instruction);
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveExternalRuntimeReachableMethods(
        AotCoreIrInstructionArtifact instruction)
    {
        var reachableMethods = new List<AotCoreIrMethodArtifact>();
        var seenSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        void AddRange(IReadOnlyList<AotCoreIrMethodArtifact> methods)
        {
            foreach (var method in methods)
            {
                if (seenSubjectIds.Add(method.SubjectId))
                {
                    reachableMethods.Add(method);
                }
            }
        }

        AddRange(ResolveDirectReachableMethods(instruction));
        if (!string.IsNullOrEmpty(instruction.Callee))
        {
            AddRange(ResolveInterfaceDispatchTargets(instruction.Callee));
        }

        return reachableMethods;
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(
        AotCoreIrInstructionArtifact instruction)
    {
        if (string.IsNullOrEmpty(instruction.Callee))
        {
            return [];
        }

        var calleeMethod = TryGetLowerableMethod(instruction.Callee);
        if (calleeMethod is not null)
        {
            return [calleeMethod];
        }

        if (TryResolveAsyncRuntimeContinuationMethod(instruction.Callee, out var continuationMethod))
        {
            return [continuationMethod!];
        }

        if (TryResolveStringJoinEnumerableHelperMethods(instruction.Callee, out var helperMethods))
        {
            return helperMethods!;
        }

        return [];
    }

    private IReadOnlyList<ExternalRuntimeHelperDefinition> CollectExternalRuntimeHelpers(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        StaticInitializationSupportModel staticInitializationSupport)
    {
        var helpersBySubjectId = new Dictionary<string, ExternalRuntimeHelperDefinition>(reachableMethods.Count, StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (string.IsNullOrEmpty(instruction.Callee) ||
                    !TryCreateExternalRuntimeHelperDefinition(instruction.Callee, out var helperDefinition))
                {
                    continue;
                }

                helpersBySubjectId[helperDefinition!.SubjectId] = helperDefinition;
            }
        }

        foreach (var subjectId in staticInitializationSupport.RequiredExternalRuntimeHelperSubjectIds)
        {
            if (TryCreateExternalRuntimeHelperDefinition(subjectId, out var helperDefinition))
            {
                helpersBySubjectId[helperDefinition!.SubjectId] = helperDefinition;
            }
        }

        return helpersBySubjectId.Values
            .OrderBy(helper => helper.TargetSymbol, StringComparer.Ordinal)
            .ToArray();
    }

    private bool TryResolveAsyncRuntimeContinuationMethod(
        string callee,
        out AotCoreIrMethodArtifact? continuationMethod)
    {
        continuationMethod = null;
        if (!TryGetAsyncStateMachineTypeName(callee, out var stateMachineTypeName) ||
            string.IsNullOrEmpty(stateMachineTypeName))
        {
            return false;
        }

        var matches = _methodsBySubjectId.Values
            .Where(method =>
                !method.IsStatic &&
                CanEmitMethodBody(method) &&
                string.Equals(GetMethodName(method.SubjectId), "MoveNext", StringComparison.Ordinal) &&
                GetMethodDeclaringTypeSubjectId(method.SubjectId).EndsWith(stateMachineTypeName, StringComparison.Ordinal))
            .OrderBy(method => method.SubjectId, StringComparer.Ordinal)
            .ToArray();
        if (matches.Length == 0)
        {
            return false;
        }

        if (matches.Length > 1)
        {
            throw new NotSupportedException(
                $"native-aot lowering found ambiguous async state-machine MoveNext targets for '{callee}'.");
        }

        continuationMethod = matches[0];
        return true;
    }

    private bool TryResolveStringJoinEnumerableHelperMethods(
        string callee,
        out IReadOnlyList<AotCoreIrMethodArtifact>? helperMethods)
    {
        helperMethods = null;

        if (TryGetStringJoinEnumerableElementType(callee, out var elementTypeDisplayName))
        {
            helperMethods = ResolveEnumerableJoinSupportMethods(elementTypeDisplayName!);
            return true;
        }

        return false;
    }

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveEnumerableJoinSupportMethods(string elementTypeDisplayName)
    {
        var methods = new List<AotCoreIrMethodArtifact>();
        var seenSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var variant in ResolveEnumerableJoinSupportVariants(elementTypeDisplayName))
        {
            if (seenSubjectIds.Add(variant.GetEnumeratorMethod.SubjectId))
            {
                methods.Add(variant.GetEnumeratorMethod);
            }

            if (seenSubjectIds.Add(variant.MoveNextMethod.SubjectId))
            {
                methods.Add(variant.MoveNextMethod);
            }

            if (seenSubjectIds.Add(variant.GetCurrentMethod.SubjectId))
            {
                methods.Add(variant.GetCurrentMethod);
            }
        }

        return methods;
    }

    private IReadOnlyList<EnumerableJoinSupportVariant> ResolveEnumerableJoinSupportVariants(string elementTypeDisplayName)
    {
        var variants = new List<EnumerableJoinSupportVariant>();

        foreach (var candidate in _referenceTypeImplementedInterfaceSubjectIds
                     .OrderBy(entry => entry.Key, StringComparer.Ordinal))
        {
            if (!ImplementsEnumerableJoinInterfaces(candidate.Value, elementTypeDisplayName))
            {
                continue;
            }

            var enumerableTypeSubjectId = candidate.Key;
            var getEnumeratorMethod = TryResolveEnumerableJoinMethod(
                enumerableTypeSubjectId,
                method =>
                    method.ParameterCount == 0 &&
                    method.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt &&
                    GetMethodName(method.SubjectId).EndsWith("GetEnumerator", StringComparison.Ordinal) &&
                    method.ReturnType.Contains($"System.Collections.Generic.IEnumerator<{elementTypeDisplayName}>", StringComparison.Ordinal));
            var moveNextMethod = TryResolveEnumerableJoinMethod(
                enumerableTypeSubjectId,
                method =>
                    method.ParameterCount == 0 &&
                    string.Equals(method.ReturnType, "System.Boolean", StringComparison.Ordinal) &&
                    GetMethodName(method.SubjectId).EndsWith("MoveNext", StringComparison.Ordinal));
            var getCurrentMethod = TryResolveEnumerableJoinMethod(
                enumerableTypeSubjectId,
                method =>
                    method.ParameterCount == 0 &&
                    string.Equals(GetTypeDisplayName(method.ReturnType), elementTypeDisplayName, StringComparison.Ordinal) &&
                    GetMethodName(method.SubjectId).EndsWith("get_Current", StringComparison.Ordinal));
            if (getEnumeratorMethod is null || moveNextMethod is null || getCurrentMethod is null)
            {
                continue;
            }

            variants.Add(new EnumerableJoinSupportVariant(
                enumerableTypeSubjectId,
                getEnumeratorMethod,
                moveNextMethod,
                getCurrentMethod));
        }

        return variants;
    }

    private AotCoreIrMethodArtifact? TryResolveEnumerableJoinMethod(
        string declaringTypeSubjectId,
        Func<AotCoreIrMethodArtifact, bool> predicate)
    {
        return _methodsBySubjectId.Values
            .Where(method =>
                string.Equals(method.Identity.DeclaringTypeSubjectId, declaringTypeSubjectId, StringComparison.Ordinal) &&
                CanEmitMethodBody(method) &&
                predicate(method))
            .OrderBy(method => method.SubjectId, StringComparer.Ordinal)
            .FirstOrDefault();
    }

    private static bool ImplementsEnumerableJoinInterfaces(
        IReadOnlySet<string> interfaceSubjectIds,
        string elementTypeDisplayName)
    {
        var implementsEnumerable = interfaceSubjectIds.Any(interfaceSubjectId =>
            TryMatchConstructedGenericInterface(
                interfaceSubjectId,
                "System.Private.CoreLib/System.Collections.Generic.IEnumerable<",
                elementTypeDisplayName));
        var implementsEnumerator = interfaceSubjectIds.Any(interfaceSubjectId =>
            TryMatchConstructedGenericInterface(
                interfaceSubjectId,
                "System.Private.CoreLib/System.Collections.Generic.IEnumerator<",
                elementTypeDisplayName));
        return implementsEnumerable && implementsEnumerator;
    }

    private static bool TryMatchConstructedGenericInterface(
        string interfaceSubjectId,
        string prefix,
        string elementTypeDisplayName)
    {
        if (!TryReadGenericArgumentList(interfaceSubjectId, prefix, out var genericArgumentList))
        {
            return false;
        }

        var arguments = SplitTopLevelGenericArguments(genericArgumentList);
        return arguments.Count == 1 &&
               string.Equals(GetTypeDisplayName(arguments[0]), elementTypeDisplayName, StringComparison.Ordinal);
    }

    private static bool TryGetStringJoinEnumerableElementType(
        string callee,
        out string? elementTypeDisplayName)
    {
        if (TryParseStringJoinGenericEnumerableElementType(callee, out elementTypeDisplayName))
        {
            return true;
        }

        if (MatchesMethodSubject(
                callee,
                StringTypeSubjectId,
                "Join",
                "System.String",
                "System.Collections.Generic.IEnumerable<System.String>"))
        {
            elementTypeDisplayName = "System.String";
            return true;
        }

        elementTypeDisplayName = null;
        return false;
    }

    private static bool TryParseStringJoinGenericEnumerableElementType(
        string callee,
        out string? elementTypeDisplayName)
    {
        elementTypeDisplayName = null;
        if (!TryReadSingleGenericTypeArgument(callee, StringJoinGenericEnumerableMethodPrefix, out var elementTypeName) ||
            !callee.Contains("(System.String,System.Collections.Generic.IEnumerable<", StringComparison.Ordinal))
        {
            return false;
        }

        elementTypeDisplayName = GetTypeDisplayName(elementTypeName);
        return !string.IsNullOrEmpty(elementTypeDisplayName);
    }

    private sealed record VirtualDispatchRoute(
        string ReceiverTypeSubjectId,
        AotCoreIrMethodArtifact ImplementationMethod);

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(
        AotCoreIrInstructionArtifact instruction)
    {
        var routes = ResolveVirtualDispatchRoutes(instruction);
        var methods = new List<AotCoreIrMethodArtifact>();
        var seenSubjectIds = new HashSet<string>(routes.Count, StringComparer.Ordinal);

        foreach (var route in routes)
        {
            if (seenSubjectIds.Add(route.ImplementationMethod.SubjectId))
            {
                methods.Add(route.ImplementationMethod);
            }
        }

        return methods;
    }

    private IReadOnlyList<VirtualDispatchRoute> ResolveVirtualDispatchRoutes(
        AotCoreIrInstructionArtifact instruction)
    {
        var dispatchSlotMethod = ResolveRequiredDispatchSlotMethod(instruction);
        var slotDeclaringTypeSubjectId = dispatchSlotMethod.Identity.DeclaringTypeSubjectId;
        var slotDeclaringTypeDefinitionSubjectId = GetDeclaringTypeSubjectId(dispatchSlotMethod.Identity.DefinitionSubjectId);
        var slotSignatureSuffix = GetMethodSignatureSuffix(dispatchSlotMethod.SubjectId);
        var routes = new List<VirtualDispatchRoute>();
        var seenReceiverTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var candidateTypeSubjectId in EnumerateVirtualDispatchCandidateTypeSubjectIds())
        {
            if (!seenReceiverTypeSubjectIds.Add(candidateTypeSubjectId))
            {
                continue;
            }

            var implementationMethod = TryResolveVirtualDispatchImplementationMethod(
                candidateTypeSubjectId,
                slotDeclaringTypeSubjectId,
                slotDeclaringTypeDefinitionSubjectId,
                slotSignatureSuffix);
            if (implementationMethod is null)
            {
                continue;
            }

            routes.Add(new VirtualDispatchRoute(candidateTypeSubjectId, implementationMethod));
        }

        return routes
            .OrderBy(route => route.ReceiverTypeSubjectId, StringComparer.Ordinal)
            .ToArray();
    }

    private IEnumerable<string> EnumerateVirtualDispatchCandidateTypeSubjectIds()
    {
        var capacity = _referenceTypeBaseSubjectIds.Count
            + _referenceTypeImplementedInterfaceSubjectIds.Count
            + _methodsBySubjectId.Count;
        var seenTypeSubjectIds = new HashSet<string>(capacity, StringComparer.Ordinal);

        foreach (var typeSubjectId in _referenceTypeBaseSubjectIds.Keys.OrderBy(subjectId => subjectId, StringComparer.Ordinal))
        {
            if (seenTypeSubjectIds.Add(typeSubjectId))
            {
                yield return typeSubjectId;
            }
        }

        foreach (var typeSubjectId in _referenceTypeImplementedInterfaceSubjectIds.Keys.OrderBy(subjectId => subjectId, StringComparer.Ordinal))
        {
            if (seenTypeSubjectIds.Add(typeSubjectId))
            {
                yield return typeSubjectId;
            }
        }

        foreach (var typeSubjectId in _methodsBySubjectId.Values
                     .Select(method => method.Identity.DeclaringTypeSubjectId)
                     .Where(subjectId => !string.IsNullOrEmpty(subjectId))
                     .OrderBy(subjectId => subjectId, StringComparer.Ordinal))
        {
            if (seenTypeSubjectIds.Add(typeSubjectId))
            {
                yield return typeSubjectId;
            }
        }
    }

    private AotCoreIrMethodArtifact? TryResolveVirtualDispatchImplementationMethod(
        string candidateTypeSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId,
        string slotSignatureSuffix)
    {
        if (!IsTypeCompatibleWithSlot(
                candidateTypeSubjectId,
                candidateTypeSubjectId,
                slotDeclaringTypeSubjectId,
                slotDeclaringTypeDefinitionSubjectId))
        {
            return null;
        }

        var currentTypeSubjectId = candidateTypeSubjectId;
        var currentTypeDefinitionSubjectId = candidateTypeSubjectId;
        while (!string.IsNullOrEmpty(currentTypeSubjectId))
        {
            if (TryResolveVirtualDispatchImplementationMethodOnType(
                    currentTypeSubjectId,
                    slotSignatureSuffix,
                    out var implementationMethod))
            {
                return implementationMethod;
            }

            if (!TryGetBaseTypeSubjectId(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    out var baseTypeSubjectId) ||
                string.IsNullOrEmpty(baseTypeSubjectId))
            {
                break;
            }

            currentTypeSubjectId = baseTypeSubjectId;
            currentTypeDefinitionSubjectId = baseTypeSubjectId;
        }

        return null;
    }

    private Dictionary<(string, string), AotCoreIrMethodArtifact>? _virtualDispatchMethodIndex;

    private bool TryResolveVirtualDispatchImplementationMethodOnType(
        string declaringTypeSubjectId,
        string slotSignatureSuffix,
        out AotCoreIrMethodArtifact? implementationMethod)
    {
        // Lazy-build the virtual dispatch method index on first call.
        var index = _virtualDispatchMethodIndex;
        if (index is null)
        {
            index = new Dictionary<(string, string), AotCoreIrMethodArtifact>();
            foreach (var method in _methodsBySubjectId.Values)
            {
                if (!method.IsStatic && CanEmitMethodBody(method))
                    index.TryAdd((method.Identity.DeclaringTypeSubjectId, GetMethodSignatureSuffix(method.SubjectId)), method);
            }
            _virtualDispatchMethodIndex = index;
        }

        var key = (declaringTypeSubjectId, slotSignatureSuffix);
        index.TryGetValue(key, out implementationMethod);
        return implementationMethod is not null;
    }

    private AotCoreIrMethodArtifact ResolveRequiredDispatchSlotMethod(AotCoreIrInstructionArtifact instruction)
    {
        var subjectId = !string.IsNullOrEmpty(instruction.Callee)
            ? instruction.Callee
            : instruction.TargetReference?.SubjectId;
        if (string.IsNullOrEmpty(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var dispatchSlotMethod))
        {
            throw new NotSupportedException(
                $"native-aot lowering does not support unresolved dispatch slot '{instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "<null>"}'.");
        }

        return dispatchSlotMethod;
    }

    private InvocationTarget ResolveDirectInvocationTarget(AotCoreIrInstructionArtifact instruction)
    {
        if (TryResolveDirectInvocationTarget(instruction.Callee) is { } directInvocationTarget)
        {
            return directInvocationTarget;
        }

        return new InvocationTarget(
            GetRequiredTargetSymbol(instruction),
            CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction)),
            CreateLegacyReturnAbiSlot(instruction.TargetReturnType),
            EmptyRawArgumentIndices,
            instruction.TargetReference?.OpenDefinitionSubjectId,
            instruction.TargetReference?.SharedGenericBodyId,
            instruction.TargetReference?.InstantiationStubId,
            instruction.TargetReference?.RuntimeGenericContext);
    }

    private InvocationTarget? TryResolveDirectInvocationTarget(string? callee)
    {
        if (string.IsNullOrEmpty(callee))
        {
            return null;
        }

        if (TryCreateExternalRuntimeHelperDefinition(callee, out var externalRuntimeHelper))
        {
            return new InvocationTarget(
                externalRuntimeHelper!.TargetSymbol,
                externalRuntimeHelper.ParameterAbis,
                externalRuntimeHelper.ReturnAbi,
                externalRuntimeHelper.RawArgumentIndices);
        }

        if (TryGetLowerableMethod(callee) is { } lowerableMethod)
        {
            return new InvocationTarget(
                TryGetInstantiationStubSymbol(lowerableMethod) ?? lowerableMethod.NativeSymbol,
                GetMethodAbiParameterSlots(lowerableMethod),
                lowerableMethod.ReturnAbi,
                EmptyRawArgumentIndices,
                lowerableMethod.OpenDefinitionSubjectId,
                lowerableMethod.SharedGenericBodyId,
                lowerableMethod.InstantiationStubId,
                lowerableMethod.RuntimeGenericContext);
        }

        return null;
    }

    private AotCoreIrMethodArtifact? TryGetLowerableMethod(string? subjectId)
    {
        if (string.IsNullOrEmpty(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !CanEmitMethodBody(method))
        {
            return null;
        }

        return method;
    }

    private bool IsTypeCompatibleWithSlot(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        var currentTypeSubjectId = candidateTypeSubjectId;
        var currentTypeDefinitionSubjectId = candidateTypeDefinitionSubjectId;
        while (!string.IsNullOrEmpty(currentTypeSubjectId))
        {
            if (MatchesTypeSubjectId(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    slotDeclaringTypeSubjectId,
                    slotDeclaringTypeDefinitionSubjectId) ||
                ImplementsInterface(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    slotDeclaringTypeSubjectId,
                    slotDeclaringTypeDefinitionSubjectId))
            {
                return true;
            }

            if (!TryGetBaseTypeSubjectId(
                    currentTypeSubjectId,
                    currentTypeDefinitionSubjectId,
                    out var baseTypeSubjectId) ||
                string.IsNullOrEmpty(baseTypeSubjectId))
            {
                break;
            }

            currentTypeSubjectId = baseTypeSubjectId;
            currentTypeDefinitionSubjectId = baseTypeSubjectId;
        }

        return false;
    }

    private bool ImplementsInterface(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return ImplementsInterface(candidateTypeSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId) ||
               ImplementsInterface(candidateTypeDefinitionSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId);
    }

    private bool ImplementsInterface(
        string candidateTypeSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        if (string.IsNullOrEmpty(candidateTypeSubjectId) ||
            !_referenceTypeImplementedInterfaceSubjectIds.TryGetValue(candidateTypeSubjectId, out var implementedInterfaceSubjectIds))
        {
            return false;
        }

        return implementedInterfaceSubjectIds.Any(interfaceSubjectId =>
            IsEquivalentTypeSubjectId(interfaceSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId));
    }

    private bool TryGetBaseTypeSubjectId(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        out string? baseTypeSubjectId)
    {
        if (TryResolveReferenceTypeBaseSubjectId(candidateTypeSubjectId, out baseTypeSubjectId))
        {
            return true;
        }

        if (TryResolveReferenceTypeBaseSubjectId(candidateTypeDefinitionSubjectId, out baseTypeSubjectId))
        {
            return true;
        }

        baseTypeSubjectId = null;
        return false;
    }

    private bool TryResolveReferenceTypeBaseSubjectId(
        string subjectId,
        out string? baseTypeSubjectId)
    {
        if (_referenceTypeBaseSubjectIds.TryGetValue(subjectId, out baseTypeSubjectId))
        {
            return true;
        }

        baseTypeSubjectId = GetSyntheticReferenceTypeBaseSubjectId(subjectId);
        return !string.IsNullOrEmpty(baseTypeSubjectId);
    }

    private static bool MatchesTypeSubjectId(
        string candidateTypeSubjectId,
        string candidateTypeDefinitionSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return IsEquivalentTypeSubjectId(candidateTypeSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId) ||
               IsEquivalentTypeSubjectId(candidateTypeDefinitionSubjectId, slotDeclaringTypeSubjectId, slotDeclaringTypeDefinitionSubjectId);
    }

    private static bool IsEquivalentTypeSubjectId(
        string? candidateTypeSubjectId,
        string slotDeclaringTypeSubjectId,
        string slotDeclaringTypeDefinitionSubjectId)
    {
        return !string.IsNullOrEmpty(candidateTypeSubjectId) &&
               (string.Equals(candidateTypeSubjectId, slotDeclaringTypeSubjectId, StringComparison.Ordinal) ||
                string.Equals(candidateTypeSubjectId, slotDeclaringTypeDefinitionSubjectId, StringComparison.Ordinal));
    }
}

