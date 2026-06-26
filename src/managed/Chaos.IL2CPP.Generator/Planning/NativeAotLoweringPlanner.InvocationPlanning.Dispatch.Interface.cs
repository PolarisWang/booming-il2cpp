using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{



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
        if (!string.IsNullOrEmpty(calleeOrTarget))
        {
            // Canonicalize assembly prefix so dictionary lookups match normalized keys.
            calleeOrTarget = NormalizeSubjectIdAssemblyCached(calleeOrTarget);
        }

        if (TryResolveDirectInvocationTarget(calleeOrTarget) is { } directInvocationTarget)
        {
            return directInvocationTarget;
        }

        // Check if the callee is in the external runtime dispatch table.
        // These are cross-assembly calls that would otherwise fall through to
        // chaos_external_runtime_* stubs — we dispatch via startup-time-resolved
        // function pointer table instead.
        string? tableKey = calleeOrTarget;
        if (!string.IsNullOrEmpty(tableKey) &&
            _externalRuntimeSubjects.TryGetValue(tableKey, out int tableIndex))
        {
            string? returnType = instruction.TargetReturnType;
            if (string.IsNullOrEmpty(returnType) && !string.IsNullOrEmpty(instruction.Callee))
                returnType = InferReturnTypeFromSubjectId(instruction.Callee);

            // Use shape registry's ParameterAbis when available for precise ABI carriers,
            // falling back to instruction metadata for methods not in the shape registry.
            IReadOnlyList<AotCoreIrAbiSlotArtifact> earlyExtParamAbis;
            IReadOnlySet<int>? earlyExtRawIndices;
            if (!string.IsNullOrEmpty(tableKey) &&
                TryCreateExternalRuntimeHelperDefinition(tableKey, out var earlyHelperDef))
            {
                earlyExtParamAbis = earlyHelperDef!.ParameterAbis.Count > 0
                    ? earlyHelperDef.ParameterAbis
                    : CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction));
                earlyExtRawIndices = earlyHelperDef.RawArgumentIndices;
            }
            else
            {
                earlyExtParamAbis = CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction));
                earlyExtRawIndices = EmptyRawArgumentIndices;
            }

            return new InvocationTarget(
                GetExternalRuntimeHelperSymbol(tableKey),
                earlyExtParamAbis,
                CreateLegacyReturnAbiSlot(returnType ?? instruction.TargetReturnType),
                earlyExtRawIndices,
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
        // When a registered helper is found, use its precise ABI carriers (ParameterAbis, ReturnAbi,
        // RawArgumentIndices) so that double-typed helpers use Float64 carrier instead of NativeInt.
        // This ensures correct ChaosLoadFloat64/ChaosStoreFloat64 marshalling at the call site —
        // without it, implicit int64→double numeric conversion corrupts bitcast double values.
        IReadOnlyList<AotCoreIrAbiSlotArtifact>? extParamAbis = null;
        AotCoreIrAbiSlotArtifact? extReturnAbi = null;
        IReadOnlySet<int>? extRawIndices = null;
        string? extDirectNativeSymbol = null;
        if (!string.IsNullOrEmpty(calleeOrTarget) &&
            TryCreateExternalRuntimeHelperDefinition(calleeOrTarget, out var helperDef))
        {
            symbol = helperDef!.TargetSymbol;
            extParamAbis = helperDef.ParameterAbis.Count > 0 ? helperDef.ParameterAbis : null;
            extReturnAbi = helperDef.ReturnAbi;
            extRawIndices = helperDef.RawArgumentIndices;
            extDirectNativeSymbol = helperDef.DirectNativeSymbol;

            // Gold Direct Link (A3): Override DirectNativeSymbol for hot methods
            // identified by PGO profiling. Use the method's actual C++ function
            // symbol to enable direct call emission instead of Demeter Table dispatch.
            if (extDirectNativeSymbol == null &&
                _goldDirectCallCache.Contains(calleeOrTarget) &&
                _methodsBySubjectId.TryGetValue(calleeOrTarget, out var goldMethod))
            {
                extDirectNativeSymbol = ResolveCallTargetNativeSymbol(goldMethod);
            }
        }
        else if (TryGetInstantiationStubSymbol(instruction.TargetReference?.InstantiationStubId) is { } stubSymbol)
        {
            symbol = stubSymbol;
        }
        else if (!string.IsNullOrEmpty(instruction.TargetSymbol) &&
                 !instruction.TargetSymbol.StartsWith("chaos_external_runtime_", StringComparison.Ordinal))
        {
            // Use TargetSymbol for non-external-runtime symbols (e.g. native function pointers).
            // For chaos_external_runtime_* symbols, derive from the normalized callee instead
            // to ensure the symbol matches the dispatch table declaration format.
            symbol = instruction.TargetSymbol;
        }
        else if (!string.IsNullOrEmpty(calleeOrTarget))
        {
            // Derive chaos_external_runtime_* from the NORMALIZED callee (without assembly
            // prefix) so it matches the dispatch table's extern "C" void declarations.
            // Using instruction.Callee (raw, non-normalized) would produce symbols with
            // assembly prefixes that don't match the normalized dispatch table entries.
            symbol = GetExternalRuntimeHelperSymbol(calleeOrTarget);
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
            extParamAbis ?? CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction)),
            extReturnAbi ?? CreateLegacyReturnAbiSlot(returnType2 ?? instruction.TargetReturnType),
            extRawIndices ?? EmptyRawArgumentIndices,
            instruction.TargetReference?.OpenDefinitionSubjectId,
            instruction.TargetReference?.SharedGenericBodyId,
            instruction.TargetReference?.InstantiationStubId,
            instruction.TargetReference?.RuntimeGenericContext,
            DirectNativeSymbol: extDirectNativeSymbol);
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
            // Gold Direct Link (A3): For hot methods identified by PGO profiling,
            // emit a direct C++ call using the method's native symbol instead of
            // going through kChaosExternalRuntimeFnTable indirect dispatch.
            // This enables C++ compiler inlining and eliminates function pointer
            // dereference overhead (~0.75ns per call).
            if (_goldDirectCallCache.Contains(callee) &&
                _methodsBySubjectId.TryGetValue(callee, out var goldMethod))
            {
                var goldSymbol = ResolveCallTargetNativeSymbol(goldMethod);
                Console.Error.WriteLine(
                    $"[GoldDirectLink] Direct call: {callee} -> {goldSymbol}");
                return new InvocationTarget(
                    helperDefinition!.TargetSymbol,
                    helperDefinition.ParameterAbis,
                    helperDefinition.ReturnAbi,
                    helperDefinition.RawArgumentIndices,
                    DirectNativeSymbol: goldSymbol);
            }

            // Extended Gold Direct Link: for ANY lowerable method that has a helper
            // definition, use the method's native AOT symbol directly.  This skips
            // the kChaosExternalRuntimeFnTable dispatch for all methods with AOT
            // bodies, not just PGO-identified hot methods.
            if (TryGetLowerableMethod(callee) is { } lowerableAotMethod &&
                helperDefinition!.DirectNativeSymbol == null)
            {
                return new InvocationTarget(
                    helperDefinition!.TargetSymbol,
                    helperDefinition.ParameterAbis,
                    helperDefinition.ReturnAbi,
                    helperDefinition.RawArgumentIndices,
                    DirectNativeSymbol: ResolveCallTargetNativeSymbol(lowerableAotMethod));
            }

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

}
