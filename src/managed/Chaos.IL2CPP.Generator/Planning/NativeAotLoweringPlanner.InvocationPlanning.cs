using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

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

    /// <summary>
    /// Returns ALL methods from the AOT Core IR (full closure mode,
    /// no entry point pruning).  Used by convert-to-cpp for full
    /// assembly translation.
    /// </summary>
    private static IReadOnlyList<AotCoreIrMethodArtifact> CollectAllMethods(
        AotCoreIrArtifact aotCoreIr)
    {
        // Filter out methods without instructions (interface declarations, abstract stubs)
        // that cannot be emitted as AOT C++ code. CCW/COM interface methods like
        // ISimpleMath::Add enter the IR with 0 instructions and will crash the emitter.
        // P/Invoke (DllImport) methods also have 0 instructions but must be kept so that
        // EmitPInvokeMethod generates their extern "C" wrapper function definitions.
        return aotCoreIr.Methods
            .Where(m => m is { Instructions.Count: > 0 } or { IsPInvoke: true })
            .OrderBy(m => ExtractNumericSortKey(m.SubjectId))
            .ThenBy(m => m.SubjectId, StringComparer.Ordinal)
            .ToList();
    }

    /// <summary>
    /// Extracts a sort key that orders methods by their numeric subject index
    /// (parsed from a trailing "Subject_N" pattern) for natural slot ordering.
    /// Methods without a matching numeric suffix sort after all numerically-keyed
    /// methods, using alphabetical fallback.
    /// </summary>
    private static (int NumericKey, string Fallback) ExtractNumericSortKey(string subjectId)
    {
        // Match trailing "Subject_NNN" pattern in the method name portion
        // (after the last "::" separator). The SubjectId format is typically:
        //   "Assembly/Type::Subject_N:Signature"
        int methodSep = subjectId.LastIndexOf("::", StringComparison.Ordinal);
        int searchStart = methodSep >= 0 ? methodSep + 2 : 0;

        // Find "Subject_" prefix
        int subjectIdx = subjectId.IndexOf("Subject_", searchStart, StringComparison.Ordinal);
        if (subjectIdx >= 0)
        {
            int numStart = subjectIdx + "Subject_".Length;
            int numEnd = numStart;
            while (numEnd < subjectId.Length && char.IsDigit(subjectId[numEnd])) numEnd++;
            if (numEnd > numStart && int.TryParse(subjectId.AsSpan(numStart, numEnd - numStart), out var num))
                return (num, subjectId);
        }

        return (int.MaxValue, subjectId);
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
                HybridDispatchKind.ComVtable => ResolveComVtableReachableMethods(instruction),
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

    private IReadOnlyList<AotCoreIrMethodArtifact> ResolveComVtableReachableMethods(
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

    /// <summary>
    /// Scans all reachable methods' instructions for cross-assembly calls that would
    /// fall through to the chaos_external_runtime_* stub path. These are callees that:
    ///   1. Are NOT in our method dictionary (cross-assembly, outside compilation closure)
    ///   2. Are NOT handled by ShapeRegistry (GenericShapeDescriptor/SimpleForward)
    ///   3. Are NOT instantiation stubs
    ///
    /// Such calls are collected into <see cref="_externalRuntimeSubjects"/> with a
    /// table index, enabling O(1) startup-time-resolved dispatch instead of per-method
    /// C++ wrapper stubs.
    /// </summary>
    private void CollectExternalRuntimeDispatchEntries(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        int nextIndex = _externalRuntimeSubjects.Count;
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
                if (string.IsNullOrEmpty(callee))
                    continue;

                // Already in method dictionary → direct call, no dispatch table needed
                if (_methodsBySubjectId.ContainsKey(callee))
                    continue;

                // ShapeRegistry/ExternalRuntimeHelper handles it → still register in
                // the dispatch table so the interpreter's ResolveDirectFn can find it.
                if (TryCreateExternalRuntimeHelperDefinition(callee, out _))
                {
                    if (_externalRuntimeSubjects.TryAdd(callee, nextIndex))
                        nextIndex++;
                    continue;
                }

                // Instantiation stub → has a definition, no table needed (same-assembly only).
                // Cross-assembly generic methods don't have stub definitions in this compilation unit
                // and must be dispatched through the external runtime table.
                if (TryGetInstantiationStubSymbol(instruction.TargetReference?.InstantiationStubId) != null &&
                    _methodsBySubjectId.ContainsKey(callee))
                    continue;

                // This callee would fall through to chaos_external_runtime_* stub generation.
                // Assign a dispatch table index so the call site can use table-indexed dispatch.
                if (!_externalRuntimeSubjects.ContainsKey(callee))
                {
                    _externalRuntimeSubjects[callee] = nextIndex++;
                }
            }
        }
    }

    /// <summary>
    /// Identifies call sites that need explicit bridge/import thunks — C++ wrapper
    /// functions that adapt managed calling conventions to native targets.
    ///
    /// These are callees currently routed through the external runtime dispatch
    /// table (<see cref="_externalRuntimeSubjects"/>) that are NOT handled by
    /// <see cref="TryCreateExternalRuntimeHelperDefinition"/> (which produces
    /// inline C++ expressions or SimpleForward stubs). For such callees, a
    /// bridge thunk provides proper GC transition and calling convention handling
    /// instead of relying on the interpreter-populated dispatch table.
    /// </summary>
    private void CollectBridgeImportThunks(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        _bridgeImportThunks ??= new Dictionary<string, BridgeImportThunkDefinition>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
                if (string.IsNullOrEmpty(callee))
                    continue;

                // Only consider callees in the external runtime dispatch table.
                if (!_externalRuntimeSubjects.ContainsKey(callee))
                    continue;

                // ExternalRuntimeHelper definitions have their own inline/shape
                // dispatch — no bridge thunk needed.
                if (TryCreateExternalRuntimeHelperDefinition(callee, out _))
                    continue;

                // Already have a bridge thunk for this callee.
                if (_bridgeImportThunks.ContainsKey(callee))
                    continue;

                // Already in the method dictionary → direct call, no bridge needed.
                if (_methodsBySubjectId.ContainsKey(callee))
                    continue;

                // Determine if this is an InternalCall (native runtime method)
                // vs a generic external call. InternalCalls are runtime methods
                // with managed signatures that need GC cooperative mode.
                bool isInternalCall = IsInternalCallSubjectId(callee);

                // For now, bridge thunks use the external runtime helper symbol
                // as the target (resolved at startup by the interpreter's table).
                // Future enhancement: resolve the actual native symbol at compile
                // time for InternalCall methods.
                var targetSymbol = GetExternalRuntimeHelperSymbol(callee);
                int paramCount = InferParameterCountFromSubjectId(callee);
                var paramAbis = new AotCoreIrAbiSlotArtifact[paramCount];
                for (int i = 0; i < paramCount; i++)
                    paramAbis[i] = CreateNativeIntAbiSlot();
                var returnType = InferReturnTypeFromSubjectId(callee);
                var returnAbi = returnType != null
                    ? CreateLegacyReturnAbiSlot(returnType)
                    : CreateVoidAbiSlot();

                _bridgeImportThunks[callee] = new BridgeImportThunkDefinition(
                    SubjectId: callee,
                    ThunkSymbol: $"chaos_bridge_thunk_{_bridgeImportThunks.Count}",
                    TargetSymbol: targetSymbol,
                    ParameterAbis: paramAbis,
                    ReturnAbi: returnAbi,
                    RequiresGcTransition: isInternalCall,
                    HasMarshalling: false,
                    IsInternalCall: isInternalCall,
                    IsPInvokeImport: false,
                    ExternalRuntimeTableIndex: _externalRuntimeSubjects.TryGetValue(callee, out var tableIdx) ? tableIdx : -1);
            }
        }
    }

    /// <summary>
    /// Heuristic: determines if a subject ID refers to an InternalCall method.
    /// InternalCall methods are runtime-internal native implementations with
    /// managed signatures (e.g., RuntimeHelpers::InitializeArray,
    /// Buffer::Memmove). They require GC cooperative mode.
    ///
    /// Detection is based on well-known patterns rather than explicit metadata,
    /// since the AOT Core IR does not carry an explicit IsInternalCall flag.
    /// </summary>
    private static bool IsInternalCallSubjectId(string subjectId)
    {
        // InternalCalls typically come from System.Private.CoreLib and have
        // well-known method names that are implemented natively in the runtime.
        if (!subjectId.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal))
            return false;

        // Common InternalCall patterns in the CHAOS runtime:
        // RuntimeHelpers.*, Buffer.*, Environment.*, etc.
        if (subjectId.Contains("::RuntimeHelpers::", StringComparison.Ordinal) ||
            subjectId.Contains("::Buffer::", StringComparison.Ordinal) ||
            subjectId.Contains("::Environment::", StringComparison.Ordinal) ||
            subjectId.Contains("::Thread::", StringComparison.Ordinal) ||
            subjectId.Contains("::Monitor::", StringComparison.Ordinal) ||
            subjectId.Contains("::Interlocked::", StringComparison.Ordinal) ||
            subjectId.Contains("::Volatile::", StringComparison.Ordinal) ||
            subjectId.Contains("::JitHelpers::", StringComparison.Ordinal) ||
            subjectId.Contains("::RuntimeImports::", StringComparison.Ordinal) ||
            subjectId.Contains("::GC::", StringComparison.Ordinal) ||
            subjectId.Contains("::Array::", StringComparison.Ordinal) ||
            subjectId.Contains("::String::", StringComparison.Ordinal) ||
            subjectId.Contains("::MemoryMarshal::", StringComparison.Ordinal) ||
            subjectId.Contains("::SpanHelpers::", StringComparison.Ordinal) ||
            subjectId.Contains("::Unsafe::", StringComparison.Ordinal))
        {
            return true;
        }

        return false;
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
                // Try Callee first (direct calls).
                string? targetSubjectId = instruction.Callee;
                if (string.IsNullOrEmpty(targetSubjectId))
                {
                    // For generic instantiation calls, Callee can be null but
                    // TargetReference carries the SubjectId and InstantiationStubId.
                    targetSubjectId = instruction.TargetReference?.SubjectId;
                }

                if (string.IsNullOrEmpty(targetSubjectId))
                {
                    continue;
                }

                if (!TryCreateExternalRuntimeHelperDefinition(targetSubjectId, out var helperDefinition))
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

        // O(1) lookup via pre-built MoveNext index, filtered by state-machine type suffix
        foreach (var kvp in _asyncMoveNextMethods)
        {
            if (kvp.Key.EndsWith(stateMachineTypeName, StringComparison.Ordinal) &&
                CanEmitMethodBody(kvp.Value))
            {
                continuationMethod = kvp.Value;
                return true;
            }
        }

        return false;
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
        if (!_methodsByDeclaringType.TryGetValue(declaringTypeSubjectId, out var methods))
            return null;

        return methods
            .Where(method => CanEmitMethodBody(method) && predicate(method))
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
        // Try Callee first, then TargetReference SubjectId for generic instantiation calls
        // where Callee may be null but the closed-form SubjectId is in TargetReference.
        string? calleeOrTarget = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (TryResolveDirectInvocationTarget(calleeOrTarget) is { } directInvocationTarget)
        {
            return directInvocationTarget;
        }

        // Check if the callee is in the external runtime dispatch table.
        // These are cross-assembly calls that would otherwise fall through to
        // chaos_external_runtime_* stubs — we dispatch via startup-time-resolved
        // function pointer table instead.
        string? tableKey = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (!string.IsNullOrEmpty(tableKey) &&
            _externalRuntimeSubjects.TryGetValue(tableKey, out int tableIndex))
        {
            string? returnType = instruction.TargetReturnType;
            if (string.IsNullOrEmpty(returnType) && !string.IsNullOrEmpty(instruction.Callee))
                returnType = InferReturnTypeFromSubjectId(instruction.Callee);

            return new InvocationTarget(
                GetExternalRuntimeHelperSymbol(tableKey),
                CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction)),
                CreateLegacyReturnAbiSlot(returnType ?? instruction.TargetReturnType),
                EmptyRawArgumentIndices,
                ExternalRuntimeTableIndex: tableIndex);
        }

        // Fallback: callee is not in our method dictionary and has no registered
        // runtime helper shape (e.g. BCL method like String.Join from a different
        // assembly not included in the closure).  Use instruction-level metadata
        // (TargetSymbol, TargetParameterCount, TargetReturnType) if present,
        // otherwise derive a callable symbol from the callee SubjectId.
        string? symbol;

        // Priority: external runtime helper (via GenericShapeDescriptor/SimpleForward)
        // before instantiation stub symbol, so that Nullable<T> etc. stub definitions
        // resolve to chaos_external_runtime_* symbols rather than undefined chaos_stub_definition_*.
        if (!string.IsNullOrEmpty(calleeOrTarget) &&
            TryCreateExternalRuntimeHelperDefinition(calleeOrTarget, out _))
        {
            symbol = GetExternalRuntimeHelperSymbol(calleeOrTarget);
        }
        else if (TryGetInstantiationStubSymbol(instruction.TargetReference?.InstantiationStubId) is { } stubSymbol)
        {
            symbol = stubSymbol;
        }
        else if (!string.IsNullOrEmpty(instruction.TargetSymbol))
        {
            symbol = instruction.TargetSymbol;
        }
        else if (!string.IsNullOrEmpty(instruction.Callee))
        {
            symbol = GetExternalRuntimeHelperSymbol(instruction.Callee);
        }
        else
        {
            throw new NotSupportedException(
                "native-aot lowering does not support unresolved call target '" +
                (instruction.Callee ?? "<null>") + "'");
        }

        string? returnType2 = instruction.TargetReturnType;
        if (string.IsNullOrEmpty(returnType2) && !string.IsNullOrEmpty(instruction.Callee))
        {
            returnType2 = InferReturnTypeFromSubjectId(instruction.Callee);
        }

        return new InvocationTarget(
            symbol,
            CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction)),
            CreateLegacyReturnAbiSlot(returnType2 ?? instruction.TargetReturnType),
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

        // Priority 1: Inline shape — emit C++ expression directly at call site,
        // before external runtime helper check, because inline expansion gives
        // the best performance (no function call at all, matches JIT inlining).
        if (_shapeRegistry.TryMatchInlineShape(callee, out var cppExpr, out var matchedDescriptor))
        {
            // Compute ABI dynamically from SubjectId instead of hardcoding.
            // Parse return type and parameter count from the SubjectId format:
            //   "Assembly/Type::MethodName:ReturnType(Param1,Param2,...)"
            var returnTypeName = InferReturnTypeFromSubjectId(callee);
            var returnAbi = returnTypeName != null
                ? CreateLegacyReturnAbiSlot(returnTypeName)
                : CreateVoidAbiSlot();

            // For instance methods, the SubjectId has 0 explicit params but the
            // `this` pointer IS on the eval stack.  Add 1 so that the emitter
            // consumes `this` from the stack and substitutes {0} in the template.
            var explicitParams = InferParameterCountFromSubjectId(callee);
            var paramCount = explicitParams + (matchedDescriptor!.IsInstanceMethod ? 1 : 0);
            var paramAbis = new AotCoreIrAbiSlotArtifact[paramCount];
            for (int i = 0; i < paramCount; i++)
                paramAbis[i] = CreateNativeIntAbiSlot();

            // All args are raw CHAOS_IL2CPP_INTPTR — consumed directly from eval stack
            // without coercion. The inline C++ expression template handles casting.
            var rawArgs = paramCount > 0
                ? new HashSet<int>(Enumerable.Range(0, paramCount))
                : EmptyRawArgumentIndices;

            return new InvocationTarget(
                TargetSymbol: callee, // unused for inline, but required for struct identity
                ParameterAbis: paramAbis,
                ReturnAbi: returnAbi,
                RawArgumentIndices: rawArgs,
                InlineCppExpression: cppExpr);
        }

        // Priority 1b: Devirtualization hint — unconditional direct call to concrete impl
        if (_devirtualizationHints.TryGetValue(callee, out var devirtHint) &&
            devirtHint.CanDevirtualize && devirtHint.GuardTypeSubjectId == null)
        {
            if (_methodsBySubjectId.TryGetValue(devirtHint.ImplementationMethodSubjectId, out var devirtMethod))
            {
                var devirtSymbol = ResolveCallTargetNativeSymbol(devirtMethod);
                var paramCount = devirtMethod.ParameterCount + (devirtMethod.IsStatic ? 0 : 1);
                var paramAbis = new AotCoreIrAbiSlotArtifact[paramCount];
                for (int i = 0; i < paramCount; i++)
                    paramAbis[i] = CreateNativeIntAbiSlot();
                var retType = InferReturnTypeFromSubjectId(devirtMethod.SubjectId);
                return new InvocationTarget(
                    TargetSymbol: devirtSymbol,
                    ParameterAbis: paramAbis,
                    ReturnAbi: CreateLegacyReturnAbiSlot(retType),
                    RawArgumentIndices: EmptyRawArgumentIndices);
            }
        }

        // Priority 2: External runtime helper (GenericShapeDescriptor or SimpleForward)
        if (TryCreateExternalRuntimeHelperDefinition(callee, out var helperDefinition))
        {
            return new InvocationTarget(
                helperDefinition!.TargetSymbol,
                helperDefinition.ParameterAbis,
                helperDefinition.ReturnAbi,
                helperDefinition.RawArgumentIndices,
                DirectNativeSymbol: helperDefinition.DirectNativeSymbol);
        }

        if (TryGetLowerableMethod(callee) is { } lowerableMethod)
        {
            // For shared generic instantiations, call the canonical body directly
            // instead of going through the per-instantiation stub. This enables
            // C++ inlining and eliminates the forwarding call overhead.
            string targetSymbol = ResolveCallTargetNativeSymbol(lowerableMethod);
            return new InvocationTarget(
                TryGetInstantiationStubSymbol(lowerableMethod) ?? targetSymbol,
                GetMethodAbiParameterSlots(lowerableMethod),
                lowerableMethod.ReturnAbi,
                EmptyRawArgumentIndices,
                lowerableMethod.OpenDefinitionSubjectId,
                lowerableMethod.SharedGenericBodyId,
                lowerableMethod.InstantiationStubId,
                lowerableMethod.RuntimeGenericContext);
        }

        // Fallback: callee exists in the closure method dictionary but was not
        // matched by external runtime helper or lowerable checks.  Use its
        // NativeSymbol and ABI so the generated C++ calls the runtime-provided
        // implementation directly (e.g., InternalCall methods, BCL intrinsics).
        if (_methodsBySubjectId.TryGetValue(callee, out var anyMethod))
        {
            string targetSymbol = ResolveCallTargetNativeSymbol(anyMethod);
            return new InvocationTarget(
                TryGetInstantiationStubSymbol(anyMethod) ?? targetSymbol,
                GetMethodAbiParameterSlots(anyMethod),
                anyMethod.ReturnAbi,
                EmptyRawArgumentIndices,
                anyMethod.OpenDefinitionSubjectId,
                anyMethod.SharedGenericBodyId,
                anyMethod.InstantiationStubId,
                anyMethod.RuntimeGenericContext);
        }

        return null;
    }

    private AotCoreIrMethodArtifact? TryGetLowerableMethod(string? subjectId)
    {
        if (string.IsNullOrEmpty(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            (!CanEmitMethodBody(method) && !method.IsPInvoke))
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

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect foldable
    /// <c>ldsfld &lt;literal_enum_field&gt;</c> → <c>call Enum::ToString()</c> patterns.
    /// Populates <see cref="_enumToStringFoldMap"/> with call-site IlOffset → field name.
    /// </summary>
    private void BuildEnumToStringFoldTable(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        _enumToStringFoldMap.Clear();

        if (_reflectionMemberSupport.FieldEntries.Count == 0)
            return;

        // Build per-type field name index for enum types
        var enumFieldNamesByType = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        foreach (var field in _reflectionMemberSupport.FieldEntries)
        {
            if (!_enumTypeSubjectIds.Contains(field.DeclaringTypeSubjectId))
                continue;
            if (!enumFieldNamesByType.TryGetValue(field.DeclaringTypeSubjectId, out var names))
            {
                names = new HashSet<string>(StringComparer.Ordinal);
                enumFieldNamesByType[field.DeclaringTypeSubjectId] = names;
            }
            names.Add(field.FieldName);
        }
        if (enumFieldNamesByType.Count == 0)
            return;

        foreach (var method in methodsForLowering)
        {
            var instrs = method.Instructions;
            if (instrs.Count == 0) continue;

            // Simulated eval stack: track which instruction index produced each slot
            var producers = new int[128];
            int depth = 0;

            for (int i = 0; i < instrs.Count; i++)
            {
                var instr = instrs[i];

                switch (instr.Op)
                {
                    case "ldsfld":
                        producers[depth++] = i;
                        break;

                    case "ldc.i4":
                    case "ldc.i8":
                    case "ldarg":
                    case "ldloc":
                    case "ldnull":
                    case "ldstr":
                    case "ldfld":
                    case "ldtoken":
                    case "ldelema":
                    case "ldlen":
                        depth++;
                        break;

                    case "stloc":
                    case "pop":
                    case "stsfld":
                    case "stfld":
                    case "stind.i":
                    case "stind.i8":
                    case "stind.i4":
                    case "stind.ref":
                        if (depth > 0) depth--;
                        break;

                    case "box":
                        // box reinterprets the value type as a reference; the underlying
                        // value (and therefore the producer instruction) is unchanged.
                        break;

                    case "dup":
                        if (depth > 0)
                        {
                            producers[depth] = producers[depth - 1];
                            depth++;
                        }
                        break;

                    case "call":
                    case "callvirt":
                        TryFoldEnumToStringCall(instr, i, instrs, producers, depth, enumFieldNamesByType);
                        // Conservative depth: pop 'this' (for instance) or 0, push result
                        if (depth > 0) depth--;
                        depth++;
                        break;

                    case "newobj":
                        if (depth > 0) depth--;
                        depth++;
                        break;

                    case "ret":
                        depth = 0;
                        break;

                    default:
                        if (instr.Op.StartsWith("br") || instr.Op == "switch")
                        {
                            if (instr.Op != "br" && depth > 0) depth--;
                        }
                        else if (instr.Op.StartsWith("st"))
                        {
                            if (depth > 0) depth--;
                        }
                        else if (instr.Op.StartsWith("ld"))
                        {
                            depth++;
                        }
                        else if (instr.Op is "add" or "sub" or "mul" or "div" or "rem"
                            or "and" or "or" or "xor" or "shl" or "shr"
                            or "ceq" or "cgt" or "clt")
                        {
                            if (depth > 1) depth--; // pop 2, push 1
                        }
                        break;
                }
            }
        }
    }

    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> instruction, check if it targets
    /// <c>System.Enum::ToString()</c> and the <c>this</c> argument was produced by
    /// an <c>ldsfld</c> of a literal enum field. If so, record the fold in
    /// <see cref="_enumToStringFoldMap"/>.
    /// </summary>
    private void TryFoldEnumToStringCall(
        AotCoreIrInstructionArtifact callInstr,
        int callIndex,
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int[] producers,
        int depth,
        Dictionary<string, HashSet<string>> enumFieldNamesByType)
    {
        var callee = callInstr.Callee;
        if (callee == null || !callee.Contains("::ToString:System.String()", StringComparison.Ordinal))
            return;

        int thisDepth = depth - 1;
        if (thisDepth < 0) return;

        int producerIdx = producers[thisDepth];
        if (producerIdx < 0 || producerIdx >= callIndex) return;

        var producer = instrs[producerIdx];
        string? fieldSubjectId = producer.Op switch
        {
            "ldsfld" => producer.TargetReference?.SubjectId,
            _ => null
        };
        if (fieldSubjectId == null) return;

        // Parse "DeclaringType::FieldName"
        var colonIdx = fieldSubjectId.LastIndexOf("::", StringComparison.Ordinal);
        if (colonIdx <= 0) return;

        var declaringType = fieldSubjectId.Substring(0, colonIdx);
        var fieldName = fieldSubjectId.Substring(colonIdx + 2);

        // Verify this is an enum literal field (not e.g. MyClass::someField)
        if (!enumFieldNamesByType.TryGetValue(declaringType, out var validNames) ||
            !validNames.Contains(fieldName))
            return;

        _enumToStringFoldMap[callInstr.IlOffset] = fieldName;
    }
}

