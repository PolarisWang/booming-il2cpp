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
        var queue = new Queue<AotCoreIrMethodArtifact>();
        queue.Enqueue(entryMethod);
        visited.Add(entryMethod.SubjectId);

        while (queue.Count > 0)
        {
            var method = queue.Dequeue();

            foreach (var instruction in method.Instructions)
            {
                foreach (var reachableMethod in ResolveReachableMethods(instruction))
                {
                    if (visited.Add(reachableMethod.SubjectId))
                        queue.Enqueue(reachableMethod);
                }
            }

            ordered.Add(method);
        }

        return ordered;
    }

    /// <summary>
    /// Returns ALL methods from the AOT Core IR (full closure mode,
    /// no entry point pruning).  Used by convert-to-cpp for full
    /// assembly translation.
    /// </summary>
    private static IReadOnlyList<AotCoreIrMethodArtifact> CollectAllMethods(
        AotCoreIrArtifact aotCoreIr,
        List<string>? preserveSubjectIds = null)
    {
        // Filter out methods without instructions (interface declarations, abstract stubs)
        // that cannot be emitted as AOT C++ code. CCW/COM interface methods like
        // ISimpleMath::Add enter the IR with 0 instructions and will crash the emitter.
        // P/Invoke (DllImport) methods also have 0 instructions but must be kept so that
        // EmitPInvokeMethod generates their extern "C" wrapper function definitions.
        // Subject methods with 0 instructions are preserved (they get a simple return body).
        return aotCoreIr.Methods
            .Where(m => (m is { Instructions.Count: > 0 } or { IsPInvoke: true })
                || (preserveSubjectIds?.Contains(m.SubjectId) == true))
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
        var _seenCallees = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
                if (string.IsNullOrEmpty(callee))
                    continue;

                // Canonicalize assembly prefix so dispatch table keys match
                // the normalized SubjectIds used by TryCreateExternalRuntimeHelperDefinition
                // and the downstream helperSymbolBySubjectId lookup.
                callee = NormalizeSubjectIdAssemblyCached(callee);

                // P0: skip already-processed callees
                if (!_seenCallees.Add(callee))
                    continue;

                // P0.5: skip type-only subjects (no ::method:) — these are JIT type-loading
                // references (e.g. constrained. calls, runtime type resolution) that should
                // NOT appear in the external runtime dispatch table.  Including them causes
                // the interpreter to crash with unhandled type-load IL when
                // ChaosExternalRuntimeFallback tries to execute the subject.
                if (!callee.Contains("::"))
                    continue;

                // Already in method dictionary with instructions -> direct AOT call, no table needed.
                bool _isAot = _methodsBySubjectId.TryGetValue(callee, out var _e) && _e is { Instructions.Count: > 0 };
                if (_isAot)
                {
                    // Try RHS for DirectNativeSymbol overrides, but don't add to dispatch table.
                    TryCreateExternalRuntimeHelperDefinition(callee, out _);
                    continue;
                }

                // ShapeRegistry/ExternalRuntimeHelper handles it -> register in dispatch table.
                // System.Console.Error.WriteLine($"[COLLECT_DEBUG] callee={callee} method={method.SubjectId}");
                if (TryCreateExternalRuntimeHelperDefinition(callee, out _))
                {
                    // System.Console.Error.WriteLine($"[COLLECT_DEBUG]   -> SHAPE FOUND for {callee}");
                    if (_externalRuntimeSubjects.TryAdd(callee, nextIndex))
                        nextIndex++;
                    continue;
                }
                // System.Console.Error.WriteLine($"[COLLECT_DEBUG]   -> NO SHAPE for {callee}");

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
        var _seenCallees = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
                if (string.IsNullOrEmpty(callee))
                    continue;

                // Canonicalize assembly prefix so dictionary keys match
                // _externalRuntimeSubjects (normalized at CollectExternalRuntimeDispatchEntries).
                callee = NormalizeSubjectIdAssemblyCached(callee);

                // P0: skip already-processed callees
                if (!_seenCallees.Add(callee))
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

                // System.Console.Error.WriteLine($"[INVOCATION_DEBUG] TryCreateExternalRuntimeHelperDefinition targetSubjectId={targetSubjectId}");
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

    private sealed record VirtualDispatchRoute(
        string ReceiverTypeSubjectId,
        AotCoreIrMethodArtifact ImplementationMethod);

    private Dictionary<(string, string), AotCoreIrMethodArtifact>? _virtualDispatchMethodIndex;

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect foldable
    /// <c>ldsfld &lt;literal_enum_field&gt;</c> → <c>call Enum::ToString()</c> patterns.
    /// Populates <see cref="_enumToStringFoldMap"/> with call-site IlOffset → field name.
    /// </summary>

    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> instruction, check if it targets
    /// <c>System.Enum::ToString()</c> and the <c>this</c> argument was produced by
    /// an <c>ldsfld</c> of a literal enum field. If so, record the fold in
    /// <see cref="_enumToStringFoldMap"/>.
    /// </summary>

    /// <summary>
    /// Pre-scan for <c>ldtoken &lt;EnumType&gt; + call Enum::Parse/Format/IsDefined</c>
    /// patterns where the enum type is known at codegen time. Records the call-site
    /// IlOffset so <c>EmitLinearCallTarget</c> can emit specialized (AOT-baked) C++
    /// strcmp-based code instead of routing through generic runtime helpers.
    /// </summary>

    /// <summary>
    /// Estimates how many arguments a call instruction pops from the eval stack.
    /// Uses the callee signature when available; falls back to conservative default.
    /// </summary>

    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> to an enum static method, check if the
    /// type argument was produced by <c>ldtoken &lt;EnumType&gt;</c>. If so, record
    /// the call site for AOT-baked specialized code emission.
    /// </summary>

    // ── A2.6: TypeInfo* direct API pre-scan ─────────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>typeof(T).IsAssignableFrom(typeof(U))</c> patterns where T and U are
    /// AOT-known types.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit
    /// <c>ChaosReflectionIsAssignableFromPtr(...)</c> directly.
    /// </summary>

    // ── A2.7: typeof(T) compile-time fold pre-scan ─────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>ldtoken &lt;const_type&gt; + call GetTypeFromHandle</c> patterns where the
    /// type is AOT-known.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit a direct TypeInfo*
    /// pointer expression.
    /// </summary>

    /// <summary>
    /// all type arguments are <c>typeof()</c> constants (ldtoken + GetTypeFromHandle).
    /// If so, produce a <see cref="TypeHierarchyPtrFoldEntry"/> that the emitter
    /// uses to emit the <c>*Ptr</c> direct API call with pre-resolved symbols.
    /// </summary>

    /// <summary>
    /// Check if <c>instrs[idx]</c> is <c>call Type::GetTypeFromHandle</c> and
    /// <c>instrs[idx-1]</c> is <c>ldtoken</c>.  If so, extract the type SubjectId.
    /// </summary>

    /// <summary>
    /// Extract the method name from a callee SubjectId like
    /// <c>"System.Type::IsAssignableFrom"</c> or
    /// <c>"System.Private.CoreLib/System.Type::IsAssignableFrom:Boolean(System.Type)"</c>.
    /// Returns the method name only if the declaring type is <c>System.Type</c>.
    /// </summary>

    /// <summary>
    /// Check if a callee SubjectId represents <c>System.Type::GetTypeFromHandle</c>.
    /// </summary>

    /// <summary>
    /// Check if a type SubjectId is AOT-known (has a <c>chaos_mt_*</c> symbol).
    /// </summary>

    /// <summary>
    /// For Enum.Format with a non-enum type (e.g. byte) and a known constant value
    /// with D/X/G format specifier, bake the result string at compile time.
    /// Non-enum types are not in _enumTypeSubjectIds so the main bake logic skips them.
    /// </summary>
}
