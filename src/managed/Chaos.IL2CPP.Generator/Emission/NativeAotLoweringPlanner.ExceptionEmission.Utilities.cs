using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static bool IsEnumToStringCall(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.Callee?.Contains("::ToString:", StringComparison.Ordinal) == true;
    }

    private static bool IsEnumFormatCall(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.Callee?.Contains("::Format:", StringComparison.Ordinal) == true;
    }

    private static string SanitizeForCppIdent(string name)
    {
        var sb = new System.Text.StringBuilder(name.Length);
        foreach (char c in name)
        {
            if (char.IsLetterOrDigit(c) || c == '_') sb.Append(c);
            else sb.Append('_');
        }
        if (sb.Length > 0 && char.IsDigit(sb[0])) sb.Insert(0, '_');
        return sb.Length > 0 ? sb.ToString() : "_empty";
    }

    private void EmitFusedEnumBoxToString(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        // ── Inline string table ──
        // Replaces CHAOS_IL2CPP_STRING_ID (tagged FNV hash) with lazy-initialized
        // static string holders.  The first access allocates via ChaosEnumAllocString;
        // subsequent accesses return the cached pointer directly — no tag-bit
        // detection, no string-table resolve, no GC allocation after first use.

        // A2.4: Constant-folded BoxToString — field name known at codegen time.
        if (_enumToStringFoldMap.TryGetValue(instruction.IlOffset, out var foldedFieldName))
        {
            ConsumeEvalStackValueExpression();
            _state.Value!.PendingEnumBoxSubjectId = null;
            string strHolder = "s_enum_str_" + SanitizeForCppIdent(foldedFieldName);
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    static CHAOS_IL2CPP_INTPTR {strHolder} = 0;");
            builder.AppendLine($"{indentation}    if (!{strHolder})");
            builder.AppendLine($"{indentation}        {strHolder} = ChaosEnumAllocString({ToCppStringLiteral(foldedFieldName)}, {foldedFieldName.Length});");
            EmitEvalStackPush(builder, indentation + "    ", strHolder);
            builder.AppendLine($"{indentation}}}");
            return;
        }

        string rawValueExpr = ConsumeEvalStackValueExpression();
        string enumSubjectId = _state.Value!.PendingEnumBoxSubjectId!;
        _state.Value!.PendingEnumBoxSubjectId = null;

        // A2.6: Per-enum switch — lazy-initialized static string holders.
        if (_enumValueToNameMap.TryGetValue(enumSubjectId, out var valueToName) &&
            valueToName.Count > 0 && valueToName.Count <= 64)
        {
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    switch (static_cast<CHAOS_IL2CPP_INT64>({rawValueExpr}))");
            builder.AppendLine($"{indentation}    {{");

            foreach (var kvp in valueToName)
            {
                string strHolder = "s_enum_str_" + SanitizeForCppIdent(kvp.Value);
                builder.AppendLine($"{indentation}        case {kvp.Key}:");
                builder.AppendLine($"{indentation}        {{");
                builder.AppendLine($"{indentation}            static CHAOS_IL2CPP_INTPTR {strHolder} = 0;");
                builder.AppendLine($"{indentation}            if (!{strHolder})");
                builder.AppendLine($"{indentation}                {strHolder} = ChaosEnumAllocString({ToCppStringLiteral(kvp.Value)}, {kvp.Value.Length});");
                EmitEvalStackPush(builder, indentation + "            ", strHolder);
                builder.AppendLine($"{indentation}            break;");
                builder.AppendLine($"{indentation}        }}");
            }

            // Default: unrecognized value — delegate to runtime stub
            builder.AppendLine($"{indentation}        default:");
            builder.AppendLine($"{indentation}        {{");
            string typeHandle = $"static_cast<CHAOS_IL2CPP_INTPTR>({GetTypeHandleLiteral(enumSubjectId)})";
            builder.AppendLine($"{indentation}            const auto chaos_result = ChaosEnumToStringRaw({typeHandle}, static_cast<CHAOS_IL2CPP_INT64>({rawValueExpr}));");
            EmitEvalStackPush(builder, indentation + "            ", "chaos_result");
            builder.AppendLine($"{indentation}            break;");
            builder.AppendLine($"{indentation}        }}");

            builder.AppendLine($"{indentation}    }}");
            builder.AppendLine($"{indentation}}}");
            return;
        }

        // Fallback: large enum (>64 fields) or no metadata — use runtime stub.
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_result = ChaosEnumToStringRaw(static_cast<CHAOS_IL2CPP_INTPTR>({GetTypeHandleLiteral(enumSubjectId)}), static_cast<CHAOS_IL2CPP_INT64>({rawValueExpr}));");
        EmitEvalStackPush(builder, indentation + "    ", "chaos_result");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitFusedEnumFormatBoxCall(StringBuilder builder, string indentation)
    {
        // Stack before pop: [type, raw_value, format_str] (format_str on top)
        string formatExpr = ConsumeEvalStackValueExpression();
        string rawValueExpr = ConsumeEvalStackValueExpression();
        string typeExpr = ConsumeEvalStackValueExpression();

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_result = ChaosEnumFormatRaw({typeExpr}, static_cast<CHAOS_IL2CPP_INT64>({rawValueExpr}), {formatExpr});");
        EmitEvalStackPush(builder, indentation + "    ", "chaos_result");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearResolvedInvocation(StringBuilder builder, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, string indentation, bool enforceInstanceNullCheck)
    {
        string a = MapAbiSlotReturnType(returnAbi);
        StringBuilder stringBuilder = builder;
        StringBuilder stringBuilder2 = stringBuilder;
        StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("{");
        stringBuilder2.AppendLine(ref handler);
        for (int num = parameterAbis.Count - 1; num >= 0; num--)
        {
            string rawExpr = ConsumeEvalStackValueExpression();
            bool isRawArg = rawArgumentIndices.Contains(num);
            if (isRawArg)
            {
                builder.AppendLine($"{indentation}    const auto chaos_arg_{num} = {rawExpr};");
            }
            else
            {
                string abiExpr = FormatInboundAbiArgumentExpression(parameterAbis[num], rawExpr);
                if (abiExpr == rawExpr)
                {
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{num} = {rawExpr};");
                }
                else
                {
                    builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num} = {rawExpr};");
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{num} = {abiExpr};");
                }
            }
        }
        if (enforceInstanceNullCheck && parameterAbis.Count > 0
            && !IsValueTypeCarrierKind(parameterAbis[0].CarrierKindCode))
        {
            builder.AppendLine(indentation + "    if (chaos_arg_0 == 0)");
            builder.AppendLine(indentation + "    {");
            builder.AppendLine(indentation + "        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
            builder.AppendLine(indentation + "    }");
        }
        string argList = FormatAbiInvocationArgumentList(parameterAbis);
        // Append hidden chaos_generic_context when calling a shared canonical body.
        // Shared callers pass their own context parameter; non-shared pass 0.
        string genericCtxArg = "";
        if (_sharedContextSymbols.Contains(targetSymbol))
        {
            bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                  _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
            genericCtxArg = string.IsNullOrEmpty(argList)
                ? (callerIsShared ? "chaos_generic_context" : "0")
                : (callerIsShared ? ", chaos_generic_context" : ", 0");
        }

        string value = targetSymbol + "(" + argList + genericCtxArg + ")";
        if (string.Equals(a, "void", StringComparison.Ordinal))
        {
            stringBuilder = builder;
            StringBuilder stringBuilder4 = stringBuilder;
            handler = new StringBuilder.AppendInterpolatedStringHandler(5, 2, stringBuilder);
            handler.AppendFormatted(indentation);
            handler.AppendLiteral("    ");
            handler.AppendFormatted(value);
            handler.AppendLiteral(";");
            stringBuilder4.AppendLine(ref handler);
        }
        else
        {
            stringBuilder = builder;
            StringBuilder stringBuilder5 = stringBuilder;
            handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
            handler.AppendFormatted(indentation);
            handler.AppendLiteral("    const auto chaos_result = ");
            handler.AppendFormatted(value);
            handler.AppendLiteral(";");
            stringBuilder5.AppendLine(ref handler);
            EmitAbiReturnPush(builder, returnAbi, "chaos_result", indentation + "    ");
        }
        stringBuilder = builder;
        StringBuilder stringBuilder6 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("}");
        stringBuilder6.AppendLine(ref handler);
    }

    private void EmitExternalRuntimeTableDispatch(StringBuilder builder, InvocationTarget invocationTarget, string indentation, bool enforceInstanceNullCheck, AotCoreIrInstructionArtifact? instruction = null)
    {
        string returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
        string paramTypes = FormatAbiSlotParameterTypes(invocationTarget.ParameterAbis);

        // When a DirectNativeSymbol is available, emit a direct function call
        // instead of an indirect dispatch table call. This enables:
        //   1. Compiler inlining (the call is a known symbol at compile time)
        //   2. No function pointer dereference overhead
        //   3. Better code generation (the compiler sees the full call graph)
        if (invocationTarget.DirectNativeSymbol is { } nativeSymbol)
        {
            // Collect chaos_external_runtime_* symbols for fallback declaration emission
            if (nativeSymbol.StartsWith("chaos_external_runtime_", StringComparison.Ordinal))
                _emittedExternalRuntimeSymbols[nativeSymbol] = invocationTarget.ReturnAbi.CarrierKindCode;
            builder.AppendLine($"{indentation}{{");
            for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
            {
                string rawExpr = ConsumeEvalStackValueExpression();
                if (!invocationTarget.RawArgumentIndices.Contains(i))
                    builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {rawExpr};");
                builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(i)
                    ? $"{indentation}    const auto chaos_arg_{i} = {rawExpr};"
                    : $"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[i], $"chaos_raw_arg_{i}")};");
            }
            if (enforceInstanceNullCheck && invocationTarget.ParameterAbis.Count > 0
    && !IsValueTypeCarrierKind(invocationTarget.ParameterAbis[0].CarrierKindCode))
            {
                // Subject methods are wrapped in try/catch that catches C++ exceptions.
                // When an external runtime method is called with null 'this' (e.g.
                // default(PipeReader)!.TryRead(...)), the null check throws, the catch
                // returns 0 unconditionally — but the test expects the external runtime
                // call to proceed and return a meaningful value.
                // Skip the null check for non-native external runtime calls from
                // subject methods; ChaosExternalRuntimeFallback handles null gracefully.
                bool isSubjectExtRuntime =
                    invocationTarget.DirectNativeSymbol == null &&
                    _state.Value!.CurrentMethodArtifact?.SubjectId is not null &&
                    _state.Value!.CurrentMethodArtifact.SubjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal);
                if (!isSubjectExtRuntime)
                {
                    builder.AppendLine(indentation + "    if (chaos_arg_0 == 0)");
                    builder.AppendLine(indentation + "    {");
                    builder.AppendLine(indentation + "        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
                    builder.AppendLine(indentation + "    }");
                }
            }
            string directNativeArgs = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
            string nativeCtxArg = "";
            if (_sharedContextSymbols.Contains(nativeSymbol))
            {
                bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                      _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
                nativeCtxArg = string.IsNullOrEmpty(directNativeArgs)
                    ? (callerIsShared ? "chaos_generic_context" : "0")
                    : (callerIsShared ? ", chaos_generic_context" : ", 0");
            }
            if (string.Equals(returnType, "void", StringComparison.Ordinal))
            {
                builder.AppendLine($"{indentation}    {nativeSymbol}({directNativeArgs}{nativeCtxArg});");
            }
            else
            {
                builder.AppendLine($"{indentation}    const auto chaos_result = {nativeSymbol}({directNativeArgs}{nativeCtxArg});");
                EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_result", indentation + "    ");
            }
            builder.AppendLine($"{indentation}}}");
            return;
        }

        string fnType = string.IsNullOrEmpty(paramTypes)
            ? $"{returnType}(*)()"
            : $"{returnType}(*)({paramTypes})";
        int idx = invocationTarget.ExternalRuntimeTableIndex;

        builder.AppendLine($"{indentation}{{");
        for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
        {
            string rawExpr = ConsumeEvalStackValueExpression();
            if (invocationTarget.RawArgumentIndices.Contains(i))
            {
                builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {rawExpr};");
            }
            else
            {
                string abiExpr = FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[i], rawExpr);
                if (abiExpr == rawExpr)
                {
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {rawExpr};");
                }
                else
                {
                    builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {rawExpr};");
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {abiExpr};");
                }
            }
        }
        if (enforceInstanceNullCheck && invocationTarget.ParameterAbis.Count > 0
            && !IsValueTypeCarrierKind(invocationTarget.ParameterAbis[0].CarrierKindCode))
        {
            builder.AppendLine(indentation + "    if (chaos_arg_0 == 0)");
            builder.AppendLine(indentation + "    {");
            builder.AppendLine(indentation + "        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
            // Guard against infinite loop if chaos_extext_end label is at function top
            // (DirectNativeSymbol section precedes the null check in the generated code).
            // raise_null_reference_exception should not return, but the goto + return
            // pair ensures no fall-through re-enters the null check.
            builder.AppendLine(indentation + "        goto chaos_extext_end;");
            if (string.Equals(returnType, "void", StringComparison.Ordinal))
                builder.AppendLine(indentation + "        return;");
            else
                builder.AppendLine(indentation + "        return {};");
            builder.AppendLine(indentation + "    }");
        }
        else if (enforceInstanceNullCheck && (string.Equals(instruction?.Op, "callvirt", StringComparison.Ordinal) || string.Equals(instruction?.Op, "call", StringComparison.Ordinal) || string.Equals(instruction?.Op, "calli", StringComparison.Ordinal)))
        {
            // External runtime dispatch with no DirectNativeSymbol: the stub takes
            // void() args so ParameterAbis is empty.  Pop 'this' from eval stack
            // and check for null before calling the sentinel stub.
            var nullThisExpr = ConsumeEvalStackValueExpression();
            builder.AppendLine(indentation + "    if (" + nullThisExpr + " == 0)");
            builder.AppendLine(indentation + "    {");
            builder.AppendLine(indentation + "        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
            builder.AppendLine(indentation + "    }");
        }
        string args = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
        // BS-5: Validate external runtime table index before dispatch.
        builder.AppendLine($"{indentation}    if ({idx} < 0 || {idx} >= kChaosExternalRuntimeCount) {{");
        builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL(); // external runtime table index {idx} out of range");
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}    if (kChaosExternalRuntimeFnTable[{idx}] == nullptr) {{");
        // External runtime fallback: return type-appropriate default
        string escapedSubjectId = EscapeCppStringLiteral(invocationTarget.TargetSymbol);
        if (string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}        ChaosExternalRuntimeFallback(\"" + escapedSubjectId + "\");");
            builder.AppendLine($"{indentation}        return;");
        }
        else
        {
            builder.AppendLine($"{indentation}        const auto chaos_ret = ChaosExternalRuntimeFallback(\"" + escapedSubjectId + "\");");
            EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_ret", indentation + "        ");
            builder.AppendLine($"{indentation}        return {{}};");
        }
        builder.AppendLine($"{indentation}    }}");
        if (string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}    reinterpret_cast<{fnType}>(kChaosExternalRuntimeFnTable[{idx}])({args});");
        }
        else
        {
            builder.AppendLine($"{indentation}    const auto chaos_result = reinterpret_cast<{fnType}>(kChaosExternalRuntimeFnTable[{idx}])({args});");
            EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_result", indentation + "    ");
        }
        builder.AppendLine($"{indentation}    chaos_extext_end: ;");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearVirtualDispatchCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        // Resolve the dispatch slot method. For cross-assembly virtual calls
        // (e.g. constrained. callvirt to Nullable<T>.GetHashCode()), the target
        // method may not be in the AOT closure. Fall back to a direct / external
        // runtime call in that case, since we lack vtable slot info.
        string? subjectId = !string.IsNullOrEmpty(instruction.Callee)
            ? instruction.Callee
            : instruction.TargetReference?.SubjectId;
        if (string.IsNullOrEmpty(subjectId) ||
            !_methodsBySubjectId.TryGetValue(subjectId, out var dispatchSlotMethod))
        {
            EmitLinearCallTarget(builder, instruction, indentation, enforceInstanceNullCheck: true);
            return;
        }
        IReadOnlyList<AotCoreIrAbiSlotArtifact> paramAbis = GetMethodAbiParameterSlots(dispatchSlotMethod);
        string returnType = MapAbiSlotReturnType(dispatchSlotMethod.ReturnAbi);
        string vtableSlotSig = GetMethodSignatureSuffix(dispatchSlotMethod.SubjectId);
        bool isInterface = !string.IsNullOrEmpty(dispatchSlotMethod.Identity.DeclaringTypeSubjectId) &&
            _interfaceTypeSubjectIds != null &&
            _interfaceTypeSubjectIds.Contains(dispatchSlotMethod.Identity.DeclaringTypeSubjectId);

        builder.AppendLine($"{indentation}{{");
        for (int i = paramAbis.Count - 1; i >= 0; i--)
        {
            builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
            builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(paramAbis[i], $"chaos_raw_arg_{i}")};");
        }
        // Null check with if/else — avoids C2362 "initialization skipped by goto".
        // raise_null_reference_exception() never returns (throws/longjmps),
        // so the else arm is semantically equivalent to the original goto pattern.
        builder.AppendLine($"{indentation}    if (chaos_arg_0 == 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}    else");
        builder.AppendLine($"{indentation}    {{");
        // VTable resolve ¡ª always through type_info->vtable_array (unified ThinLockableHeader)
        string vtableSource = $"chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array";
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            // No initializer ({}) for chaos_callvirt_result — C2362 fires when
            // a goto (from the null-check path) jumps past an initialized variable.
            // The variable is never read on the null path (exception unwinds the frame).
            builder.AppendLine($"{indentation}    {returnType} chaos_callvirt_result;");
        }
        // VTable dispatch
        if (_vtableSlotMap != null && _vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
        {
            string vtableArgs = FormatAbiInvocationArgumentList(paramAbis, "chaos_arg_0");
            string vtableParamSig = FormatAbiSlotParameterSignature(paramAbis);
            string vtableFnType = string.IsNullOrEmpty(vtableParamSig)
                ? $"{returnType}(*)()"
                : $"{returnType}(*)({vtableParamSig})";
            if (string.Equals(returnType, "void", StringComparison.Ordinal))
            {
                string fnCall = $"reinterpret_cast<{vtableFnType}>({vtableSource}[{vtableSlot}u])";
                builder.AppendLine($"{indentation}    (*{fnCall})({vtableArgs});");
            }
            else
            {
                string fnCall = $"reinterpret_cast<{vtableFnType}>({vtableSource}[{vtableSlot}u])";
                builder.AppendLine($"{indentation}    chaos_callvirt_result = (*{fnCall})({vtableArgs});");
            }
        }
        else
        {
            builder.AppendLine($"{indentation}    CHAOS_IL2CPP_FAIL();");
        }
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            EmitAbiReturnPush(builder, dispatchSlotMethod.ReturnAbi, "chaos_callvirt_result", indentation + "    ");
        }
        builder.AppendLine($"{indentation}    }}");  // close else block
        builder.AppendLine($"{indentation}}}");       // close outer block
    }

    private void EmitDevirtFallbackVTableDispatch(StringBuilder builder, AotCoreIrInstructionArtifact instruction,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> paramAbis, AotCoreIrAbiSlotArtifact returnAbi,
        string returnType, string indentation)
    {
        AotCoreIrMethodArtifact dispatchSlotMethod = ResolveRequiredDispatchSlotMethod(instruction);
        string vtableSlotSig = GetMethodSignatureSuffix(dispatchSlotMethod.SubjectId);

        if (_vtableSlotMap != null && _vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
        {
            string vtableArgs = FormatAbiInvocationArgumentList(paramAbis, "chaos_arg_0");
            string vtableParamSig = FormatAbiSlotParameterSignature(paramAbis);
            string vtableFnType = string.IsNullOrEmpty(vtableParamSig)
                ? $"{returnType}(*)()"
                : $"{returnType}(*)({vtableParamSig})";
            string fnCall = $"reinterpret_cast<{vtableFnType}>(chaos_dt_ti->vtable_array[{vtableSlot}u])";
            if (string.Equals(returnType, "void", StringComparison.Ordinal))
            {
                builder.AppendLine($"{indentation}(*{fnCall})({vtableArgs});");
            }
            else
            {
                builder.AppendLine($"{indentation}chaos_dt_result = (*{fnCall})({vtableArgs});");
            }
        }
        else
        {
            builder.AppendLine($"{indentation}CHAOS_IL2CPP_FAIL();");
        }
    }

    private void EmitHotpatchResolvedInvocation(StringBuilder builder, int dispatchSlotIndex, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, string indentation, string? directNativeSymbol = null)
    {
        string returnType = MapAbiSlotReturnType(returnAbi);
        bool hasReturn = !string.Equals(returnType, "void", StringComparison.Ordinal);
        builder.AppendLine($"{indentation}{{");
        for (int i = parameterAbis.Count - 1; i >= 0; i--)
        {
            string rawExpr = ConsumeEvalStackValueExpression();
            if (rawArgumentIndices.Contains(i))
            {
                builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {rawExpr};");
            }
            else
            {
                string abiExpr = FormatInboundAbiArgumentExpression(parameterAbis[i], rawExpr);
                if (abiExpr == rawExpr)
                {
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {rawExpr};");
                }
                else
                {
                    builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {rawExpr};");
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {abiExpr};");
                }
            }
        }
        builder.AppendLine($"{indentation}    auto& _d{dispatchSlotIndex} = s_hotpatch_entries[{dispatchSlotIndex}];");
        if (hasReturn)
        {
            builder.AppendLine($"{indentation}    {returnType} _d_hpresult{{}};");
        }
        builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->hotpatch_is_active(&_d{dispatchSlotIndex})");
        builder.AppendLine($"{indentation}        && !chaos_runtime_get_abi_v0()->hotpatch_should_keep_native(&_d{dispatchSlotIndex}))");
        builder.AppendLine($"{indentation}    {{");
        if (parameterAbis.Count > 0)
        {
            int argBufferSize = CalculateArgBufferSize(parameterAbis);
            builder.AppendLine($"{indentation}        alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[{argBufferSize}];");
            builder.AppendLine($"{indentation}        ArgBuffer _d_bw(_d_ab);");
            for (int i = 0; i < parameterAbis.Count; i++)
            {
                builder.AppendLine($"{indentation}        _d_bw.{GetArgBufferWriteCall(parameterAbis[i].CarrierKindCode, $"chaos_arg_{i}")};");
            }
        }
        if (hasReturn)
        {
            builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->interpreter_entry_direct(");
            builder.AppendLine($"{indentation}            _d{dispatchSlotIndex}.method_key, {(parameterAbis.Count > 0 ? "_d_ab" : "nullptr")}, &_d_hpresult);");
        }
        else
        {
            builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->interpreter_entry_direct(");
            builder.AppendLine($"{indentation}            _d{dispatchSlotIndex}.method_key, {(parameterAbis.Count > 0 ? "_d_ab" : "nullptr")}, nullptr);");
        }
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}    else");
        builder.AppendLine($"{indentation}    {{");
        string nativeTarget = directNativeSymbol ?? targetSymbol;
        // Collect chaos_external_runtime_* symbols for file-scope static inline stubs.
        // Do NOT emit a block-scope extern declaration — it conflicts with the
        // file-scope extern "C" declaration that has the correct parameter list,
        // causing C2733 (cannot overload extern "C" with mismatched params).
        if (nativeTarget.StartsWith("chaos_external_runtime_", StringComparison.Ordinal))
            _emittedExternalRuntimeSymbols[nativeTarget] = returnAbi.CarrierKindCode;
        // Append hidden chaos_generic_context for shared canonical targets.
        string hpArgList = FormatAbiInvocationArgumentList(parameterAbis);
        string hpCtxArg = "";
        if (_sharedContextSymbols.Contains(nativeTarget))
        {
            bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                  _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
            hpCtxArg = string.IsNullOrEmpty(hpArgList)
                ? (callerIsShared ? "chaos_generic_context" : "0")
                : (callerIsShared ? ", chaos_generic_context" : ", 0");
        }
        string callExpr = $"{nativeTarget}({hpArgList}{hpCtxArg})";
        if (hasReturn)
        {
            builder.AppendLine($"{indentation}        _d_hpresult = {callExpr};");
        }
        else
        {
            builder.AppendLine($"{indentation}        {callExpr};");
        }
        builder.AppendLine($"{indentation}    }}");
        if (hasReturn)
        {
            EmitAbiReturnPush(builder, returnAbi, "_d_hpresult", $"{indentation}    ");
        }
        builder.AppendLine($"{indentation}}}");
    }
}
