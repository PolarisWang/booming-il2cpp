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
            _pendingEnumBoxSubjectId = null;
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
        string enumSubjectId = _pendingEnumBoxSubjectId!;
        _pendingEnumBoxSubjectId = null;

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
            bool callerIsShared = _currentMethodNativeSymbol != null &&
                                  _sharedContextSymbols.Contains(_currentMethodNativeSymbol);
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
                    _currentMethodArtifact?.SubjectId is not null &&
                    _currentMethodArtifact.SubjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal);
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
                bool callerIsShared = _currentMethodNativeSymbol != null &&
                                      _sharedContextSymbols.Contains(_currentMethodNativeSymbol);
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
        builder.AppendLine($"{indentation}        const auto chaos_ret = ChaosExternalRuntimeFallback(\"" + escapedSubjectId + "\");");
        EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_ret", indentation + "        ");
        if (string.Equals(returnType, "void", StringComparison.Ordinal))
            builder.AppendLine($"{indentation}        return;");
        else
            builder.AppendLine($"{indentation}        return {{}};");
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
        // Null check
        builder.AppendLine($"{indentation}    if (chaos_arg_0 == 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
        builder.AppendLine($"{indentation}        goto chaos_vcall_end_{instruction.IlOffset};");
        builder.AppendLine($"{indentation}    }}");
        // VTable resolve ¡ª always through type_info->vtable_array (unified ThinLockableHeader)
        string vtableSource = $"chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array";
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}    {returnType} chaos_callvirt_result{{}};");
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
            builder.AppendLine($"{indentation}    chaos_vcall_end_{instruction.IlOffset}: ;");
        }
        builder.AppendLine($"{indentation}}}");
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
            bool callerIsShared = _currentMethodNativeSymbol != null &&
                                  _sharedContextSymbols.Contains(_currentMethodNativeSymbol);
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

    private void EmitLinearBinaryArithmetic(StringBuilder builder, string indentation, string helperName)
    {
        SlotType _rType = PeekSlotType();
        string _rExpr = ConsumeEvalStackValueExpression();
        ConsumeSlotType();
        SlotType _lType = PeekSlotType();
        string _lExpr = ConsumeEvalStackValueExpression();
        ConsumeSlotType();

        // When either operand is float/double, emit direct C++ arithmetic
        // instead of ChaosWrap* helpers (which truncate through int32).
        bool _rIsFloat = _rType is SlotType.Float32 or SlotType.Float64;
        bool _lIsFloat = _lType is SlotType.Float32 or SlotType.Float64;
        bool isFloatOp = _lIsFloat || _rIsFloat;
        bool _rIsInt64 = _rType is SlotType.Int64;
        bool _lIsInt64 = _lType is SlotType.Int64;
        bool isInt64Op = _lIsInt64 || _rIsInt64;

        // In structured mode, _dN/_fN typed slots are already float/double C++ vars -> use directly
        string _rLoad = isFloatOp
            ? (_rIsFloat
                ? (_activeStructuredSlotContext is not null && (_rExpr.StartsWith("_d", StringComparison.Ordinal) || _rExpr.StartsWith("_f", StringComparison.Ordinal))
                    ? _rExpr
                    : $"ChaosLoadFloat64({_rExpr})")
                : $"static_cast<double>({_rExpr})")
            : isInt64Op
                ? (_rIsInt64
                    ? $"ChaosLoadInt64({_rExpr})"
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_rExpr}))")
                : $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})";
        string _lLoad = isFloatOp
            ? (_lIsFloat
                ? (_activeStructuredSlotContext is not null && (_lExpr.StartsWith("_d", StringComparison.Ordinal) || _lExpr.StartsWith("_f", StringComparison.Ordinal))
                    ? _lExpr
                    : $"ChaosLoadFloat64({_lExpr})")
                : $"static_cast<double>({_lExpr})")
            : isInt64Op
                ? (_lIsInt64
                    ? $"ChaosLoadInt64({_lExpr})"
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_lExpr}))")
                : $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})";

        if (_activeStructuredSlotContext is not null)
        {
            if (isFloatOp)
            {
                string floatExpr = helperName switch
                {
                    "ChaosWrapAdd" => $"({_lLoad} + {_rLoad})",
                    "ChaosWrapSub" => $"({_lLoad} - {_rLoad})",
                    "ChaosWrapMul" => $"({_lLoad} * {_rLoad})",
                    "ChaosDiv" => $"({_lLoad} / {_rLoad})",
                    "ChaosRem" => $"fmod({_lLoad}, {_rLoad})",
                    _ => throw new NotSupportedException()
                };
                EmitEvalStackPush(builder, indentation, floatExpr, SlotType.Float64);
            }
            else if (isInt64Op)
            {
                string _rLoadInt64 = _rIsInt64
                    ? (_activeStructuredSlotContext is not null && _rExpr.StartsWith("_i", StringComparison.Ordinal)
                        ? _rExpr
                        : $"ChaosLoadInt64({_rExpr})")
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_rExpr}))";
                string _lLoadInt64 = _lIsInt64
                    ? (_activeStructuredSlotContext is not null && _lExpr.StartsWith("_i", StringComparison.Ordinal)
                        ? _lExpr
                        : $"ChaosLoadInt64({_lExpr})")
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_lExpr}))";
                string int64Expr = helperName switch
                {
                    "ChaosWrapAdd" => $"{_lLoadInt64} + {_rLoadInt64}",
                    "ChaosWrapSub" => $"{_lLoadInt64} - {_rLoadInt64}",
                    "ChaosWrapMul" => $"{_lLoadInt64} * {_rLoadInt64}",
                    _ => $"{helperName}({_lLoadInt64}, {_rLoadInt64})",
                };
                // In structured mode, EmitEvalStackPush with SlotType.Int64 allocates _iN
                // which is already CHAOS_IL2CPP_INT64 (int64_t), so no ChaosStoreInt64 wrapper needed.
                // In pc-dispatch mode, ChaosStoreInt64 is still needed.
                if (_activeStructuredSlotContext is not null)
                    EmitEvalStackPush(builder, indentation, int64Expr, SlotType.Int64);
                else
                    EmitEvalStackPush(builder, indentation, $"ChaosStoreInt64({int64Expr})", SlotType.Int64);
            }
            else
            {
                // For int32 add/sub/mul, emit direct C++ wrapping expressions
                // using raw eval-stack expressions with a single (uint32) truncation
                // instead of the redundant (uint32)(int32) double-cast chain.
                // The outer (int32) +(intptr) preserves signed wrap-on-overflow semantics.
                // ChaosDiv/ChaosRem still need function dispatch (divide-by-zero check).
                string expr = helperName switch
                {
                    "ChaosWrapAdd" => $"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>({_lExpr}) + static_cast<CHAOS_IL2CPP_UINT32>({_rExpr})))",
                    "ChaosWrapSub" => $"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>({_lExpr}) - static_cast<CHAOS_IL2CPP_UINT32>({_rExpr})))",
                    "ChaosWrapMul" => $"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>({_lExpr}) * static_cast<CHAOS_IL2CPP_UINT32>({_rExpr})))",
                    _ => $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}({_lLoad}, {_rLoad}))",
                };
                EmitEvalStackPush(builder, indentation, expr);
            }
        }
        else
        {
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
            builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
            string expr = isFloatOp
                ? BuildFloatArithmeticExpression(helperName, "chaos_left", "chaos_right")
                : isInt64Op
                    ? $"ChaosStoreInt64({helperName}(chaos_left, chaos_right))"
                    : $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}(chaos_left, chaos_right))";
            EmitEvalStackPush(builder, indentation + "    ", expr);
            builder.AppendLine($"{indentation}}}");
        }
        PushSlotType(isFloatOp ? SlotType.Float64 : isInt64Op ? SlotType.Int64 : SlotType.NativeInt);
    }

    private static string BuildFloatArithmeticExpression(string helperName, string left, string right)
    {
        return helperName switch
        {
            "ChaosWrapAdd" => $"static_cast<CHAOS_IL2CPP_INTPTR>(({left} + {right}))",
            "ChaosWrapSub" => $"static_cast<CHAOS_IL2CPP_INTPTR>(({left} - {right}))",
            "ChaosWrapMul" => $"static_cast<CHAOS_IL2CPP_INTPTR>(({left} * {right}))",
            "ChaosDiv" => $"static_cast<CHAOS_IL2CPP_INTPTR>(({left} / {right}))",
            "ChaosRem" => $"static_cast<CHAOS_IL2CPP_INTPTR>(fmod({left}, {right}))",
            _ => throw new NotSupportedException($"Float arithmetic helper '{helperName}' not supported")
        };
    }

    private void EmitLinearBinaryBitwise(StringBuilder builder, string indentation, string operation)
    {
        ConsumeSlotType();
        string _rExpr = ConsumeEvalStackValueExpression();
        ConsumeSlotType();
        string _lExpr = ConsumeEvalStackValueExpression();
        if (_activeStructuredSlotContext is not null)
        {
            EmitEvalStackPush(builder, indentation,
                $"chaos_store_uint64(chaos_load_uint64({_lExpr}) {operation} chaos_load_uint64({_rExpr}))");
        }
        else
        {
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    const auto chaos_right = chaos_load_uint64({_rExpr});");
            builder.AppendLine($"{indentation}    const auto chaos_left = chaos_load_uint64({_lExpr});");
            EmitEvalStackPush(builder, indentation + "    ", $"chaos_store_uint64(chaos_left {operation} chaos_right)");
            builder.AppendLine($"{indentation}}}");
        }
        PushSlotType(SlotType.NativeInt);
    }

    private void EmitLinearFieldLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
        {
            throw new NotSupportedException($"native-aot structured EH linear field load requires field target reference, got '{requiredTargetReference.Kind}'.");
        }
        StringBuilder stringBuilder;
        StringBuilder.AppendInterpolatedStringHandler handler;
        if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
        {
            EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, indentation);
            // Register static field for extern declaration (emission generates chaos_static_* references not in AotCoreIr)
            if (_staticFieldDeclarations == null)
                _staticFieldDeclarations = new Dictionary<string, string?>(System.StringComparer.Ordinal);
            _staticFieldDeclarations.TryAdd(requiredTargetReference.SubjectId, requiredTargetReference.FieldTypeSubjectId);
            stringBuilder = builder;
            EmitEvalStackPush(builder, indentation, GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
            return;
        }
        stringBuilder = builder;
        StringBuilder stringBuilder3 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("{");
        stringBuilder3.AppendLine(ref handler);
        if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
        {
            string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
            builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
            EmitEvalStackPush(builder, indentation + "    ", $"chaos_value->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)}");
        }
        else
        {
            string requiredDeclaringTypeSubjectId2 = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
            builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(requiredDeclaringTypeSubjectId2)}*>({ConsumeEvalStackValueExpression()});");
            EmitEvalStackPush(builder, indentation + "    ", $"chaos_object->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)}");
        }
        stringBuilder = builder;
        StringBuilder stringBuilder8 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("}");
        stringBuilder8.AppendLine(ref handler);
    }

    private void EmitLinearLoadStringLiteral(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        string requiredStringOperand = GetRequiredStringOperand(instruction);

        // When StringId mapping is available, emit a tagged StringId (zero allocation).
        // The runtime string_table resolves it, and all downstream consumers
        // (stfld, stind.ref, stelem.ref, delegate invoke, external runtime helpers)
        // already materialize StringId via chaos_runtime_get_abi_v0()->string_materialize() or handle it internally.
        if (_stringIdMapping is { Count: > 0 } && TryGetStringId(requiredStringOperand, out _))
        {
            builder.AppendLine($"{indentation}{{");
            EmitEvalStackPush(builder, indentation + "    ", $"CHAOS_IL2CPP_STRING_ID({ToCppStringLiteral(requiredStringOperand)})");
            builder.AppendLine($"{indentation}}}");
            return;
        }

        StringBuilder stringBuilder = builder;
        StringBuilder stringBuilder2 = stringBuilder;
        StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("{");
        stringBuilder2.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder3 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(46, 2, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    auto* chaos_string = CHAOS_IL2CPP_NEW_GC(");
        handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
        handler.AppendLiteral(");");
        stringBuilder3.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder4 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(36, 2, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    chaos_string->header.type_info = ");
        handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));
        handler.AppendLiteral(";");
        stringBuilder4.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder5 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(56, 2, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(");
        handler.AppendFormatted(Encoding.UTF8.GetByteCount(requiredStringOperand));
        handler.AppendLiteral(");");
        stringBuilder5.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder6 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    chaos_string->utf8_data = ");
        handler.AppendFormatted(ToCppStringLiteral(requiredStringOperand));
        handler.AppendLiteral(";");
        stringBuilder6.AppendLine(ref handler);
        EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string)");
        stringBuilder = builder;
        StringBuilder stringBuilder8 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("}");
        stringBuilder8.AppendLine(ref handler);
    }

    private void EmitLinearLoadTypeToken(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        EmitEvalStackPush(builder, indentation, GetRequiredTypeHandleLiteral(instruction));
    }

    private void EmitLinearShift(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string helperName)
    {
        RequireInt32IntegralResultType(instruction);
        ConsumeSlotType();
        string _shiftExpr = ConsumeEvalStackValueExpression();
        ConsumeSlotType();
        string _valueExpr = ConsumeEvalStackValueExpression();
        if (_activeStructuredSlotContext is not null)
        {
            EmitEvalStackPush(builder, indentation,
                $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}(static_cast<CHAOS_IL2CPP_INT32>({_valueExpr}), static_cast<CHAOS_IL2CPP_INT32>({_shiftExpr})))");
        }
        else
        {
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    const auto chaos_shift = static_cast<CHAOS_IL2CPP_INT32>({_shiftExpr});");
            builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>({_valueExpr});");
            EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}(chaos_value, chaos_shift))");
            builder.AppendLine($"{indentation}}}");
        }
        PushSlotType(SlotType.NativeInt);
    }

    private void EmitLinearBitwiseNot(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        RequireInt32IntegralResultType(instruction);
        if (_activeStructuredSlotContext is not null)
        {
            string expr = AccessEvalStackTopExpression();
            builder.AppendLine($"{indentation}{expr} = static_cast<CHAOS_IL2CPP_INTPTR>(~static_cast<CHAOS_IL2CPP_INT32>({expr}));");
        }
        else
        {
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>({AccessEvalStackTopExpression()});");
            builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(~chaos_value);");
            builder.AppendLine($"{indentation}}}");
        }
    }

    private void EmitLinearComparisonResult(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string operandType, string comparisonOperator)
    {
        RequireInt32IntegralResultType(instruction);
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_right = static_cast<{operandType}>({ConsumeEvalStackValueExpression()});");
        builder.AppendLine($"{indentation}    const auto chaos_left = static_cast<{operandType}>({ConsumeEvalStackValueExpression()});");
        EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left {comparisonOperator} chaos_right ? 1 : 0)");
        builder.AppendLine($"{indentation}}}");
    }

    private static IReadOnlyList<int> GetRequiredSwitchTargets(AotCoreIrInstructionArtifact instruction, IReadOnlySet<int> offsets)
    {
        object? operand = instruction.Operand;
        IReadOnlyList<int> readOnlyList2;
        if (!(operand is int[] array))
        {
            if (!(operand is IReadOnlyList<int> readOnlyList))
            {
                if (!(operand is JsonElement { ValueKind: JsonValueKind.Array } jsonElement))
                {
                    throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32[] operand for native-aot lowering");
                }
                readOnlyList2 = jsonElement.EnumerateArray().Select(delegate (JsonElement targetElement)
                {
                    if (targetElement.ValueKind != JsonValueKind.Number || !targetElement.TryGetInt32(out var value))
                    {
                        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32[] operand for native-aot lowering");
                    }
                    return value;
                }).ToArray();
            }
            else
            {
                readOnlyList2 = readOnlyList;
            }
        }
        else
        {
            readOnlyList2 = array;
        }
        IReadOnlyList<int> readOnlyList3 = readOnlyList2;
        // Filter out targets that don't correspond to known IL offsets.
        // Some IL switch instructions (e.g. from FSharp.Core or optimized
        // compiler output) include targets to offsets outside the method body.
        readOnlyList3 = readOnlyList3.Where(offsets.Contains).ToArray();
        return readOnlyList3;
    }

    private static int GetRequiredIlOffset(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.IlOffset;
    }

    private static int GetRequiredIntOperand(AotCoreIrInstructionArtifact instruction)
    {

        // Short-form opcode handling: ldarg.N, ldloc.N, stloc.N
        if (instruction.Op is "ldarg.0" or "ldarg.1" or "ldarg.2" or "ldarg.3" or
            "ldloc.0" or "ldloc.1" or "ldloc.2" or "ldloc.3" or
            "stloc.0" or "stloc.1" or "stloc.2" or "stloc.3")
            return instruction.Op[^1] - '0';
        object? operand = instruction.Operand;
        if (operand is int)
        {
            return (int)operand;
        }
        if (operand is sbyte sb) return sb;
        if (operand is byte ub) return ub;
        if (operand is short ss) return ss;
        if (operand is ushort us) return us;
        if (operand is long l) return (int)l;
        if (operand is ulong ul) return (int)ul;
        if (operand is uint ui) return (int)ui;
        if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetInt32(out var value))
        {
            return value;
        }
        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32 operand for native-aot lowering");
    }

    private static void RequireInt32IntegralResultType(AotCoreIrInstructionArtifact instruction)
    {
        if (string.Equals(instruction.ResultType, "System.Int32", StringComparison.Ordinal) || string.Equals(instruction.ResultType, "System.UInt32", StringComparison.Ordinal))
        {
            return;
        }
        throw new NotSupportedException($"native-aot lowering does not support opcode '{instruction.Op}' for result type '{instruction.ResultType ?? "<null>"}'.");
    }

    private static long GetRequiredInt64Operand(AotCoreIrInstructionArtifact instruction)
    {
        object? operand = instruction.Operand;
        if (operand is long)
        {
            return (long)operand;
        }
        if (instruction.Operand is int num)
        {
            return num;
        }
        if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetInt64(out var value))
        {
            return value;
        }
        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int64 operand for native-aot lowering");
    }

    private static float GetRequiredSingleOperand(AotCoreIrInstructionArtifact instruction)
    {
        object? operand = instruction.Operand;
        if (operand is float)
        {
            return (float)operand;
        }
        if (instruction.Operand is double num)
        {
            return (float)num;
        }
        if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetSingle(out var value))
        {
            return value;
        }
        // Fallback: try converting from JsonElement string or other numeric types.
        if (operand is JsonElement je && je.ValueKind == JsonValueKind.String &&
            float.TryParse(je.GetString(), System.Globalization.NumberStyles.Any,
                System.Globalization.CultureInfo.InvariantCulture, out var parsedSingle))
        {
            return parsedSingle;
        }
        if (operand is long l)
        {
            return (float)l;
        }
        if (operand is int i)
        {
            return (float)i;
        }
        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a Single operand for native-aot lowering");
    }

    private static double GetRequiredDoubleOperand(AotCoreIrInstructionArtifact instruction)
    {
        object? operand = instruction.Operand;
        if (operand is double)
        {
            return (double)operand;
        }
        if (instruction.Operand is float num)
        {
            return num;
        }
        if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetDouble(out var value))
        {
            return value;
        }
        // Fallback: try converting from JsonElement string or other numeric types.
        if (operand is JsonElement je && je.ValueKind == JsonValueKind.String &&
            double.TryParse(je.GetString(), System.Globalization.NumberStyles.Any,
                System.Globalization.CultureInfo.InvariantCulture, out var parsedDouble))
        {
            return parsedDouble;
        }
        if (operand is long l)
        {
            return (double)l;
        }
        if (operand is int i)
        {
            return (double)i;
        }
        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a Double operand for native-aot lowering");
    }

    private static string BuildBridgeParamPassing(IReadOnlyList<AotCoreIrAbiSlotArtifact> paramAbis)
    {
        if (paramAbis.Count == 0)
            return string.Empty;

        var names = new string[paramAbis.Count];
        for (int i = 0; i < paramAbis.Count; i++)
            names[i] = "chaos_fn_arg_" + i.ToString();
        return string.Join(", ", names);
    }

    private static bool IsConvertToCharObjectCall(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.Callee?.Contains("::ToChar:System.Char(System.Object)", System.StringComparison.Ordinal) == true;
    }

    private static bool IsConvertToCharObjectProviderCall(AotCoreIrInstructionArtifact instruction)
    {
        return instruction.Callee?.Contains("::ToChar:System.Char(System.Object,System.IFormatProvider)", System.StringComparison.Ordinal) == true;
    }

    private static string ExtractTypeName(string subjectId)
    {
        var slashIdx = subjectId.LastIndexOf('/');
        return slashIdx >= 0 ? subjectId.Substring(slashIdx + 1) : subjectId;
    }

    private static string GetConvertCharNativeFunctionName(string typeSubjectId) => typeSubjectId switch
    {
        "System.Byte" => "chaos_convert_tochar_byte",
        "System.SByte" => "chaos_convert_tochar_sbyte",
        "System.Int16" => "chaos_convert_tochar_int16",
        "System.Int32" => "chaos_convert_tochar_int32",
        "System.Int64" => "chaos_convert_tochar_int64",
        "System.UInt16" => "chaos_convert_tochar_uint16",
        "System.UInt32" => "chaos_convert_tochar_uint32",
        "System.UInt64" => "chaos_convert_tochar_uint64",
        "System.Single" => "chaos_convert_tochar_single",
        "System.Double" => "chaos_convert_tochar_double",
        "System.Boolean" => "chaos_convert_tochar_boolean",
        "System.Char" => "chaos_convert_tochar_char",
        _ => throw new System.ArgumentException($"Unsupported type: {typeSubjectId}")
    };

    private void EmitFusedConvertCharBoxCall(System.Text.StringBuilder builder, string indentation)
    {
        // If this was an IFormatProvider overload, pop the provider null value
        if (_pendingBoxHasProvider)
        {
            ConsumeEvalStackValueExpression();
            _pendingBoxHasProvider = false;
        }
        string rawValueExpr = ConsumeEvalStackValueExpression();
        string inlineExpr = _pendingBoxSubjectId switch
        {
            "System.Byte" or "System.Char" or "System.UInt16"
                => $"static_cast<CHAOS_IL2CPP_UINT16>({rawValueExpr})",
            "System.SByte"
                => $"({rawValueExpr} < 0 ? (chaos_runtime_get_abi_v0()->raise_exception(0), static_cast<CHAOS_IL2CPP_UINT16>(0)) : static_cast<CHAOS_IL2CPP_UINT16>({rawValueExpr}))",
            "System.Int16" or "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64"
                => $"(({rawValueExpr} < 0 || {rawValueExpr} > 0xFFFF) ? (chaos_runtime_get_abi_v0()->raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<CHAOS_IL2CPP_UINT16>(0)) : static_cast<CHAOS_IL2CPP_UINT16>({rawValueExpr}))",
            "System.Boolean" or "System.DateTime"
                => $"(chaos_runtime_get_abi_v0()->raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)), static_cast<CHAOS_IL2CPP_UINT16>(0))",
            _ => null!
        };

        if (inlineExpr != null)
        {
            EmitEvalStackPush(builder, indentation, "static_cast<CHAOS_IL2CPP_INTPTR>(" + inlineExpr + ")");
        }
        else
        {
            string nativeFn = GetConvertCharNativeFunctionName(_pendingBoxSubjectId!);
            builder.AppendLine(indentation + "{");
            builder.AppendLine(indentation + "    const auto chaos_result = " + nativeFn + "(" + rawValueExpr + ");");
            EmitEvalStackPush(builder, indentation + "    ", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result)");
            builder.AppendLine(indentation + "}");
        }
        _pendingBoxSubjectId = null;
    }

    private static bool IsValueTypeCarrierKind(AotCoreIrAbiCarrierKind kind) => kind switch
    {
        AotCoreIrAbiCarrierKind.Int32 => true,
        AotCoreIrAbiCarrierKind.Int64 => true,
        AotCoreIrAbiCarrierKind.UInt64 => true,
        AotCoreIrAbiCarrierKind.Int8 => true,
        AotCoreIrAbiCarrierKind.UInt8 => true,
        AotCoreIrAbiCarrierKind.Int16 => true,
        AotCoreIrAbiCarrierKind.UInt16 => true,
        AotCoreIrAbiCarrierKind.Float32 => true,
        AotCoreIrAbiCarrierKind.Float64 => true,
        AotCoreIrAbiCarrierKind.Void => true,
        _ => false,
    };
}
