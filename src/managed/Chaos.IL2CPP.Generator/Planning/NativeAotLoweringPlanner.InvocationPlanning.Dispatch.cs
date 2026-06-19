using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

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

        // ── Phase 3: AOT Devirtualization ──
        {
            var key = instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "";
            if (key.Length > 0)
            {
                // (a) Sealed class: declaring type is sealed, only one possible implementation
                bool isSealedType = _sealedTypeSubjectIds != null
                    && _sealedTypeSubjectIds.Contains(slotDeclaringTypeSubjectId);
                if (isSealedType)
                {
                    var sealedImpl = TryResolveVirtualDispatchImplementationMethod(
                        slotDeclaringTypeSubjectId,
                        slotDeclaringTypeSubjectId,
                        slotDeclaringTypeDefinitionSubjectId,
                        slotSignatureSuffix);
                    if (sealedImpl != null)
                    {
                        _devirtualizationHints[key] = new DevirtualizationHint(
                            true,
                            sealedImpl.SubjectId,
                            sealedImpl.Identity.DeclaringTypeSubjectId);
                    }
                }
                else if (_interfaceTypeSubjectIds != null &&
                    _interfaceTypeSubjectIds.Contains(slotDeclaringTypeSubjectId))
                {
                    // Interface dispatch: devirtualize only with guard check
                    var ifaceUniqueImpls = routes
                        .Select(r => r.ImplementationMethod.SubjectId)
                        .Distinct(StringComparer.Ordinal)
                        .ToArray();
                    if (ifaceUniqueImpls.Length == 1)
                    {
                        // Single implementation — guard on first receiver type, or unconditional if sealed
                        string guardReceiver = _sealedTypeSubjectIds != null
                            && _sealedTypeSubjectIds.Contains(routes[0].ReceiverTypeSubjectId)
                            ? null!
                            : routes[0].ReceiverTypeSubjectId;
                        _devirtualizationHints[key] = new DevirtualizationHint(
                            true,
                            routes[0].ImplementationMethod.SubjectId,
                            routes[0].ImplementationMethod.Identity.DeclaringTypeSubjectId,
                            guardTypeSubjectId: guardReceiver);
                    }
                    // Multiple implementations: skip devirtualization, fall through to vtable
                }
                else
                {
                    // (b) Guard-based: exactly 2 implementations, one from declaring type itself
                    var uniqueImpls = routes
                        .Select(r => r.ImplementationMethod.SubjectId)
                        .Distinct(StringComparer.Ordinal)
                        .ToArray();
                    if (uniqueImpls.Length == 2)
                    {
                        // Find the implementation that belongs to the declaring type itself
                        var declaringImpl = routes.FirstOrDefault(r =>
                            string.Equals(r.ReceiverTypeSubjectId, slotDeclaringTypeSubjectId, StringComparison.Ordinal));
                        if (declaringImpl != null)
                        {
                            string guardType3c = _sealedTypeSubjectIds != null
                                && _sealedTypeSubjectIds.Contains(slotDeclaringTypeSubjectId)
                                ? null!
                                : slotDeclaringTypeSubjectId;
                            _devirtualizationHints[key] = new DevirtualizationHint(
                                true,
                                declaringImpl.ImplementationMethod.SubjectId,
                                declaringImpl.ImplementationMethod.Identity.DeclaringTypeSubjectId,
                                guardTypeSubjectId: guardType3c);
                        }
                    }
                    else if (uniqueImpls.Length == 1)
                    {
                        // (c) Monomorphic: all routes resolve to the same implementation
                        _devirtualizationHints[key] = new DevirtualizationHint(
                            true,
                            routes[0].ImplementationMethod.SubjectId,
                            routes[0].ImplementationMethod.Identity.DeclaringTypeSubjectId);
                    }
                }
            }
        }

        // ── Phase 4: Reachability-based devirtualization ──
        // If Phase 3 did not produce a hint (multiple implementations exist in metadata),
        // check whether only ONE implementation's declaring type is actually instantiated
        // in the compiled closure. If so, devirtualize to that implementation.
        var p4Key = instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "";
        if (p4Key.Length > 0 && !_devirtualizationHints.ContainsKey(p4Key))
        {
            var instantiatedRoutes = routes
                .Where(r => _instantiatedTypeSubjectIds.Contains(r.ReceiverTypeSubjectId))
                .ToList();

            if (instantiatedRoutes.Count == 1)
            {
                var impl = instantiatedRoutes[0].ImplementationMethod;
                _devirtualizationHints[p4Key] = new DevirtualizationHint(
                    true,
                    impl.SubjectId,
                    impl.Identity.DeclaringTypeSubjectId);
            }
            else if (instantiatedRoutes.Count > 1)
            {
                // Multiple instantiated implementations — guard against the slot's declaring type
                var declaringImpl = instantiatedRoutes.FirstOrDefault(r =>
                    string.Equals(r.ReceiverTypeSubjectId, slotDeclaringTypeSubjectId,
                        StringComparison.Ordinal));
                if (declaringImpl != null)
                {
                    string guardType4 = _sealedTypeSubjectIds != null
                        && _sealedTypeSubjectIds.Contains(slotDeclaringTypeSubjectId)
                        ? null!
                        : slotDeclaringTypeSubjectId;
                    _devirtualizationHints[p4Key] = new DevirtualizationHint(
                        true,
                        declaringImpl.ImplementationMethod.SubjectId,
                        declaringImpl.ImplementationMethod.Identity.DeclaringTypeSubjectId,
                        guardTypeSubjectId: guardType4);
                }
            }
        }

        return routes
            .OrderBy(route => route.ReceiverTypeSubjectId, StringComparer.Ordinal)
            .ToArray();
    }



    private IEnumerable<string> EnumerateVirtualDispatchCandidateTypeSubjectIds()
    {
        return _allDeclaringTypeSubjectIds.OrderBy(subjectId => subjectId, StringComparer.Ordinal);
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

}
