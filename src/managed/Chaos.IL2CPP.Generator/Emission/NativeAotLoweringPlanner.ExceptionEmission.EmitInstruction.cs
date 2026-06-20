using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{



    private void EmitInstructionSequence(StringBuilder builder, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, string indentation)
    {
        ResetArrayCheckCache();
        var filtered = FilterRedundantStoreReloadPairs(instructions);
        _linearInstructionList = filtered;
        for (int i = 0; i < filtered.Count; i++)
        {
            _linearInstructionIndex = i;
            var instruction = filtered[i];
            var nextInstruction = (i + 1 < filtered.Count) ? filtered[i + 1] : null;
            EmitInstruction(builder, instruction, indentation, nextInstruction);
        }
        _linearInstructionList = null;
    }





    /// <summary>
    /// Emit a pre-filtered instruction list with next-instruction lookahead.
    /// Unlike EmitInstructionSequence, this does NOT call
    /// FilterRedundantStoreReloadPairs — the caller is responsible for
    /// any filtering.  Used by the structured IR path which already
    /// filters before each loop.
    /// </summary>
    private void EmitInstructionLookahead(StringBuilder builder, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, string indentation)
    {
        _lookaheadInstructionList = instructions;
        int _lookaheadSkipCount = 0;
        for (int i = 0; i < instructions.Count; i++)
        {
            _lookaheadInstructionIndex = i;
            var instr = instructions[i];
            var nextInstr = (i + 1 < instructions.Count) ? instructions[i + 1] : null;
            try
            {
                EmitInstruction(builder, instr, indentation, nextInstr);
            }
            catch (Exception ex) when (ex is NotSupportedException or InvalidOperationException)
            {
                // Suppress — these are handled by BuildMethodSourceSafe's outer catch.
                // Logging here would flood stderr with thousands of lines, filling the
                // pipe buffer and causing STATUS_HEAP_CORRUPTION (0xC000037D).
            }
        }
        if (_lookaheadSkipCount > 0)
            Console.Error.WriteLine($"[codegen] INFO: {_lookaheadSkipCount} instruction(s) skipped in {_currentMethodArtifact?.SubjectId ?? "?"} (will fall back to stub)");
        _lookaheadInstructionList = null;
    }





    /// <summary>
    /// Resolve the effective next instruction for box elimination lookahead.
    /// When <paramref name="nextInstruction"/> is provided (linear path), use it directly.
    /// When null (structured IR path), fall back to the lookahead context fields.
    /// </summary>
    private bool TryGetLookaheadInstruction(AotCoreIrInstructionArtifact? nextInstruction, [System.Diagnostics.CodeAnalysis.NotNullWhen(true)] out AotCoreIrInstructionArtifact? result)
    {
        if (nextInstruction != null) { result = nextInstruction; return true; }
        if (_lookaheadInstructionList != null)
        {
            int nextIdx = _lookaheadInstructionIndex + 1;
            if (nextIdx < _lookaheadInstructionList.Count)
            {
                result = _lookaheadInstructionList[nextIdx];
                return true;
            }
        }
        result = null;
        return false;
    }





    /// <summary>
    /// Two-step lookahead for patterns spanning 3 instructions (box + ldnull + call).
    /// Only used in structured IR path; linear path uses nextInstruction directly.
    /// </summary>
    private bool TryGetSecondLookaheadInstruction(AotCoreIrInstructionArtifact? nextInstruction, [System.Diagnostics.CodeAnalysis.NotNullWhen(true)] out AotCoreIrInstructionArtifact? result)
    {
        // Linear path: use _linearInstructionList with _linearInstructionIndex
        if (nextInstruction != null && _linearInstructionList != null)
        {
            int nextIdx = _linearInstructionIndex + 2;
            if (nextIdx < _linearInstructionList.Count)
            {
                result = _linearInstructionList[nextIdx];
                return true;
            }
        }
        // Structured IR path: use _lookaheadInstructionList
        if (_lookaheadInstructionList != null)
        {
            int nextIdx = _lookaheadInstructionIndex + 2;
            if (nextIdx < _lookaheadInstructionList.Count)
            {
                result = _lookaheadInstructionList[nextIdx];
                return true;
            }
        }
        result = null;
        return false;
    }





    private void EmitStructuredFinallyHandlerSequence(StringBuilder builder, FinallyHandlerShape handlerShape, string indentation)
    {
        if (!TryCreateFinallyHandlerEmissionPlan(handlerShape, out FinallyHandlerEmissionPlan? emissionPlan) || emissionPlan is null)
        {
            throw new NotSupportedException($"native-aot finally handler at IL offset {handlerShape.ExceptionRegion.HandlerOffset} does not support current structured emission shape");
        }
        if (emissionPlan.Guard is null)
        {
            EmitInstructionSequence(builder, emissionPlan.BodyInstructions, indentation);
            return;
        }
        EmitInstructionSequence(builder, emissionPlan.Guard.ConditionInstructions, indentation);
        StringBuilder stringBuilder = builder;
        StringBuilder stringBuilder2 = stringBuilder;
        StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("{");
        stringBuilder2.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder3 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(77, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    const auto chaos_finally_condition = chaos_eval_stack[--chaos_stack_top];");
        stringBuilder3.AppendLine(ref handler);
        builder.AppendLine(emissionPlan.Guard.BranchWhenNonZeroToEnd ? (indentation + "    if (chaos_finally_condition == 0)") : (indentation + "    if (chaos_finally_condition != 0)"));
        stringBuilder = builder;
        StringBuilder stringBuilder4 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(5, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    {");
        stringBuilder4.AppendLine(ref handler);
        EmitInstructionSequence(builder, emissionPlan.BodyInstructions, indentation + "        ");
        stringBuilder = builder;
        StringBuilder stringBuilder5 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(5, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        handler.AppendLiteral("    }");
        stringBuilder5.AppendLine(ref handler);
        stringBuilder = builder;
        StringBuilder stringBuilder6 = stringBuilder;
        handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
        handler.AppendFormatted(indentation);
        builder.AppendLine($"{indentation}    chaos_func_end: ;");
        handler.AppendLiteral("}");
        stringBuilder6.AppendLine(ref handler);
    }





    // ©¤©¤ Shared throw/rethrow emission helpers ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
    // Consolidated from three duplicate implementations:
    //   EmitInstruction (structured EH linear)
    //   EmitFlatGotoBody (flat goto fallback)
    //   EmitIRBlockTerminator (structured IR)

    private void EmitThrowCpp(StringBuilder builder, string throwValueExpression, string indentation)
    {
        ResetArrayCheckCache();
        builder.AppendLine($"{indentation}CHAOS_EH_THROW({throwValueExpression});");
    }





    private void EmitRethrowCpp(StringBuilder builder, string indentation)
    {
        builder.AppendLine($"{indentation}CHAOS_EH_RETHROW;");
    }





    private void CompensateDceSkipForStructuredSlots(AotCoreIrInstructionArtifact instruction)
    {
        if (_activeStructuredSlotContext == null) return;
        // Call instructions (GetTypeFromHandle) have net 0 stack effect.
        if (instruction.Op == "call" || instruction.Op == "callvirt") return;
        // Estimate net stack effect for other DCE-skipped instructions.
        int net = EstimatePushCount(instruction.Op) - EstimatePopCount(instruction.Op);
        if (net > 0)
        {
            for (int i = 0; i < net; i++)
                _activeStructuredSlotContext.AllocatePushTarget();
        }
        else if (net < 0)
        {
            _activeStructuredSlotContext.Discard(-net);
        }
    }





    private void EmitInstruction(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, AotCoreIrInstructionArtifact? nextInstruction = null)
    {
        // A2.6 DCE: Skip dead ltoken + GetTypeFromHandle when TypeInfo* fold fires.
        if (_typeHierarchyPtrSkipIlOffsets.Count > 0 &&
            _currentMethodNativeSymbol != null &&
            (instruction.Op == "ldtoken" || instruction.Op == "call") &&
            _typeHierarchyPtrSkipIlOffsets.TryGetValue(_currentMethodNativeSymbol, out var skipOffsets) &&
            skipOffsets.Contains(instruction.IlOffset))
        {
            CompensateDceSkipForStructuredSlots(instruction);
            return;
        }
        // A2.6 DCE: Skip dead instructions when enum AOT bake fires.
        if (_enumAotBakeSkipIlOffsets.Count > 0 &&
            _currentMethodNativeSymbol != null &&
            _enumAotBakeSkipIlOffsets.TryGetValue(_currentMethodNativeSymbol, out var enumSkipOffsets) &&
            enumSkipOffsets.Contains(instruction.IlOffset))
        {
            CompensateDceSkipForStructuredSlots(instruction);
            return;
        }
        // A2.7 DCE: Skip dead ltoken when typeof(T) compile-time fold fires.
        if (_typeOfSkipIlOffsets.Count > 0 &&
            _currentMethodNativeSymbol != null &&
            instruction.Op == "ldtoken" &&
            _typeOfSkipIlOffsets.TryGetValue(_currentMethodNativeSymbol, out var typeOfSkipSet) &&
            typeOfSkipSet.Contains(instruction.IlOffset))
        {
            CompensateDceSkipForStructuredSlots(instruction);
            return;
        }

        switch (instruction.Op)
        {
            case "ldc.i4":
                {
                    EmitEvalStackPush(builder, indentation, $"static_cast<CHAOS_IL2CPP_INTPTR>({FormatInt32Literal(GetRequiredIntOperand(instruction))})");
                    break;
                }
            case "ldc.i8":
                {
                    string _i8lit = FormatInt64Literal(GetRequiredInt64Operand(instruction));
                    if (_activeStructuredSlotContext is not null)
                        EmitEvalStackPush(builder, indentation, _i8lit, SlotType.Int64);
                    else
                        EmitEvalStackPush(builder, indentation, $"ChaosStoreInt64({_i8lit})");
                    PushSlotType(SlotType.Int64);
                    break;
                }
            case "ldc.r8":
                {
                    EmitEvalStackPush(builder, indentation, FormatFloat64Literal(GetRequiredDoubleOperand(instruction)), SlotType.Float64);
                    PushSlotType(SlotType.Float64);
                    break;
                }
            case "ldc.r4":
                {
                    EmitEvalStackPush(builder, indentation, FormatFloat32Literal(GetRequiredSingleOperand(instruction)), SlotType.Float32);
                    PushSlotType(SlotType.Float32);
                    break;
                }
            case "ldarg":
                {
                    EmitEvalStackPush(builder, indentation, $"chaos_args[{GetRequiredIntOperand(instruction)}]");
                    PushSlotType(SlotType.NativeInt);
                    break;
                }
            case "ldstr":
                EmitLinearLoadStringLiteral(builder, instruction, indentation);
                break;
            case "ldtoken":
                EmitLinearLoadTypeToken(builder, instruction, indentation);
                break;
            case "ldloc":
                {
                    int ldlocSlot = GetRequiredIntOperand(instruction);
                    // E6: hoisted invariant local check (before all other branches)
                    if (_hoistedInvariantLocals is not null && _hoistedInvariantLocals.TryGetValue(ldlocSlot, out var _hldInfo))
                    {
                        EmitEvalStackPush(builder, indentation, _hldInfo.VarName, _hldInfo.SlotType);
                        PushSlotType(_hldInfo.SlotType);
                        break;
                    }
                    // E7: accumulator slot check (bypasses chaos_locals load)
                    if (_accumulatorSlots is not null && _accumulatorSlots.TryGetValue(ldlocSlot, out var _accName))
                    {
                        EmitEvalStackPush(builder, indentation, _accName, SlotType.Int64);
                        PushSlotType(SlotType.Int64);
                        break;
                    }
                    if (_structLocalSlots is not null && _structLocalSlots.Contains(ldlocSlot))
                        EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[{ldlocSlot}])");
                    else if (_floatLocalSlots is not null && _floatLocalSlots.TryGetValue(ldlocSlot, out var ldlocType) && ldlocType != SlotType.NativeInt)
                    {
                        string wrapper = ldlocType switch
                        {
                            SlotType.Float32 => "ChaosLoadFloat32",
                            SlotType.Float64 => "ChaosLoadFloat64",
                            _ => throw new NotSupportedException(),
                        };
                        EmitEvalStackPush(builder, indentation, $"{wrapper}(chaos_locals[{ldlocSlot}])", ldlocType);
                        PushSlotType(ldlocType);
                    }
                    else if (_int64LocalSlots is not null && _int64LocalSlots.Contains(ldlocSlot))
                    {
                        EmitEvalStackPush(builder, indentation, $"ChaosLoadInt64(chaos_locals[{ldlocSlot}])", SlotType.Int64);
                        PushSlotType(SlotType.Int64);
                    }
                    else
                    {
                        string _ldSrc = _hoistedIVs is not null && _hoistedIVs.TryGetValue(ldlocSlot, out var _ivName)
                            ? _ivName
                            : $"chaos_locals[{ldlocSlot}]";
                        EmitEvalStackPush(builder, indentation, _ldSrc);
                        // Track slot variable → chaos_locals mapping for array base hoisting
                        if (_slotVarToLocalSlot is not null && _activeStructuredSlotContext is not null && _ldSrc.StartsWith("chaos_locals[", StringComparison.Ordinal))
                            _slotVarToLocalSlot[AccessEvalStackTopExpression()] = ldlocSlot;
                        PushSlotType(SlotType.NativeInt);
                    }
                    break;
                }
            case "ldsfld":
            case "ldfld":
                EmitLinearFieldLoad(builder, instruction, indentation);
                break;
            case "call":
                if (_pendingBoxSubjectId != null && (IsConvertToCharObjectCall(instruction) || IsConvertToCharObjectProviderCall(instruction)))
                {
                    EmitFusedConvertCharBoxCall(builder, indentation);
                }
                else if (_pendingBoxSubjectId != null && IsEnumFormatCall(instruction))
                {
                    EmitFusedEnumFormatBoxCall(builder, indentation);
                    _pendingBoxSubjectId = null;
                }
                else if (_pendingEnumBoxSubjectId != null && IsEnumToStringCall(instruction))
                {
                    EmitFusedEnumBoxToString(builder, instruction, indentation);
                    _pendingEnumBoxSubjectId = null;
                }
                else
                    EmitLinearCall(builder, instruction, indentation);
                break;
            case "callvirt":
                if (_pendingBoxSubjectId != null && (IsConvertToCharObjectCall(instruction) || IsConvertToCharObjectProviderCall(instruction)))
                {
                    EmitFusedConvertCharBoxCall(builder, indentation);
                }
                else if (_pendingBoxSubjectId != null && IsEnumFormatCall(instruction))
                {
                    EmitFusedEnumFormatBoxCall(builder, indentation);
                    _pendingBoxSubjectId = null;
                }
                else if (_pendingEnumBoxSubjectId != null && IsEnumToStringCall(instruction))
                {
                    EmitFusedEnumBoxToString(builder, instruction, indentation);
                    _pendingEnumBoxSubjectId = null;
                }
                else
                    EmitLinearCallVirt(builder, instruction, indentation);
                break;
            case "newobj":
                EmitLinearNewObject(builder, instruction, indentation);
                break;
            case "ldftn":
                {
                    var targetSymbol = GetRequiredFunctionPointerTargetSymbol(instruction);

                    // If the target has a hotpatch dispatch slot, emit a forwarding thunk
                    // so that ldftn/delegate invocations go through hotpatch-aware dispatch.
                    if (_nativeSymbolToDispatchSlot?.TryGetValue(targetSymbol, out int ftnSlot) == true
                        && instruction.Callee != null
                        && _methodsBySubjectId.TryGetValue(instruction.Callee, out var ftnMethod))
                    {
                        var ftnParams = GetMethodAbiParameterSlots(ftnMethod);
                        string ftnRet = MapAbiSlotReturnType(ftnMethod.ReturnAbi);
                        bool ftnHasReturn = !string.Equals(ftnRet, "void", StringComparison.Ordinal);
                        string ftnSig = FormatAbiSlotParameterSignature(ftnParams);
                        string ftnTypes = FormatAbiSlotParameterTypes(ftnParams);
                        string ftnAbSize = ftnParams.Count > 0 ? CalculateArgBufferSize(ftnParams).ToString() : "0";

                        builder.AppendLine($"{indentation}{{");
                        builder.AppendLine($"{indentation}    // Hotpatch-aware ldftn wrapper (slot {ftnSlot})");
                        builder.AppendLine($"{indentation}    static auto* chaos_ftn_thunk = +[]({ftnSig}) -> {ftnRet} {{");
                        builder.AppendLine($"{indentation}        auto& _d_entry = s_hotpatch_entries[{ftnSlot}];");
                        builder.AppendLine($"{indentation}        if (chaos_runtime_get_abi_v0()->hotpatch_is_active(&_d_entry)");
                        builder.AppendLine($"{indentation}            && !chaos_runtime_get_abi_v0()->hotpatch_should_keep_native(&_d_entry))");
                        builder.AppendLine($"{indentation}        {{");
                        if (ftnParams.Count > 0)
                        {
                            builder.AppendLine($"{indentation}            alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[{ftnAbSize}];");
                            builder.AppendLine($"{indentation}            ArgBuffer _d_bw(_d_ab);");
                            for (int i = 0; i < ftnParams.Count; i++)
                            {
                                builder.AppendLine($"{indentation}            _d_bw.{GetArgBufferWriteCall(ftnParams[i].CarrierKindCode, $"chaos_fn_arg_{i}")};");
                            }
                        }
                        string ftnAb = ftnParams.Count > 0 ? "_d_ab" : "nullptr";
                        string directCallArgs = string.Join(", ", Enumerable.Range(0, ftnParams.Count).Select(i => $"chaos_fn_arg_{i}"));
                        if (ftnHasReturn)
                        {
                            builder.AppendLine($"{indentation}            {ftnRet} _d_ret{{}};");
                            builder.AppendLine($"{indentation}            chaos_runtime_get_abi_v0()->interpreter_entry_direct(");
                            builder.AppendLine($"{indentation}                _d_entry.method_key, {ftnAb}, &_d_ret);");
                            builder.AppendLine($"{indentation}            return _d_ret;");
                        }
                        else
                        {
                            builder.AppendLine($"{indentation}            chaos_runtime_get_abi_v0()->interpreter_entry_direct(");
                            builder.AppendLine($"{indentation}                _d_entry.method_key, {ftnAb}, nullptr);");
                            builder.AppendLine($"{indentation}            return;");
                        }
                        builder.AppendLine($"{indentation}        }}");
                        if (ftnHasReturn)
                        {
                            builder.AppendLine($"{indentation}        return reinterpret_cast<{ftnRet}(*)({ftnTypes})>(_d_entry.direct_ptr)({directCallArgs});");
                        }
                        else
                        {
                            builder.AppendLine($"{indentation}        reinterpret_cast<void(*)({ftnTypes})>(_d_entry.direct_ptr)({directCallArgs});");
                        }
                        builder.AppendLine($"{indentation}    }};");
                        EmitEvalStackPush(builder, indentation, "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk)");
                        builder.AppendLine($"{indentation}}}");
                    }
                    else
                    {
                        EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{targetSymbol})");
                    }
                    break;
                }
            case "stloc":
                ResetArrayCheckCache();
                {
                    SlotType stlocType = PeekSlotType();
                    string stlocValue = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    string storedExpr = stlocType switch
                    {
                        SlotType.Float32 => $"ChaosStoreFloat32({stlocValue})",
                        SlotType.Float64 => $"ChaosStoreFloat64({stlocValue})",
                        SlotType.Int64 => $"ChaosStoreInt64({stlocValue})",
                        _ => stlocValue,
                    };
                    builder.AppendLine($"{indentation}chaos_locals[{GetRequiredIntOperand(instruction)}] = {storedExpr};");
                    // If this slot is a hoisted IV, also update the C++ local
                    int _stlocSlot = GetRequiredIntOperand(instruction);
                    // E7: accumulator slot write-back
                    if (_accumulatorSlots is not null && _accumulatorSlots.TryGetValue(_stlocSlot, out var _accName))
                    {
                        builder.AppendLine($"{indentation}{_accName} = {stlocValue};");
                    }
                    if (_hoistedIVs is not null && _hoistedIVs.TryGetValue(_stlocSlot, out var _ivName))
                        builder.AppendLine($"{indentation}{_ivName} = {storedExpr};");
                    break;
                }
            case "starg":
                ResetArrayCheckCache();
                {
                    builder.AppendLine($"{indentation}chaos_args[{GetRequiredIntOperand(instruction)}] = {ConsumeEvalStackValueExpression()};");
                    break;
                }
            case "pop":
                {
                    EmitEvalStackDiscard(builder, indentation);
                    break;
                }
            case "ldnull":
                {
                    EmitEvalStackPush(builder, indentation, "0");
                    break;
                }
            case "cgt.un":
                {
                    string _rExpr = ConsumeEvalStackValueExpression();
                    string _lExpr = ConsumeEvalStackValueExpression();
                    if (_activeStructuredSlotContext is not null)
                    {
                        EmitEvalStackPush(builder, indentation,
                            $"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({_lExpr}) > static_cast<CHAOS_IL2CPP_UINTPTR>({_rExpr}) ? 1 : 0)");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}{{");
                        builder.AppendLine($"{indentation}    const auto chaos_right = static_cast<CHAOS_IL2CPP_UINTPTR>({_rExpr});");
                        builder.AppendLine($"{indentation}    const auto chaos_left = static_cast<CHAOS_IL2CPP_UINTPTR>({_lExpr});");
                        EmitEvalStackPush(builder, indentation + "    ", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0)");
                        builder.AppendLine($"{indentation}}}");
                    }
                    break;
                }
            case "ceq":
                {
                    SlotType _rType = PeekSlotType();
                    string _rExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    SlotType _lType = PeekSlotType();
                    string _lExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    string _rLoad = _rType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_rExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_rExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INTPTR>({_rExpr})",
                    };
                    string _lLoad = _lType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_lExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_lExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INTPTR>({_lExpr})",
                    };
                    if (_activeStructuredSlotContext is not null)
                    {
                        EmitEvalStackPush(builder, indentation,
                            $"static_cast<CHAOS_IL2CPP_INTPTR>({_lLoad} == {_rLoad} ? 1 : 0)");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}{{");
                        builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
                        builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
                        EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0)");
                        builder.AppendLine($"{indentation}}}");
                    }
                    PushSlotType(SlotType.NativeInt);
                    break;
                }
            case "cgt":
                {
                    SlotType _rType = PeekSlotType();
                    string _rExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    SlotType _lType = PeekSlotType();
                    string _lExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    string _rLoad = _rType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_rExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_rExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})",
                    };
                    string _lLoad = _lType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_lExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_lExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})",
                    };
                    if (_activeStructuredSlotContext is not null)
                    {
                        EmitEvalStackPush(builder, indentation,
                            $"static_cast<CHAOS_IL2CPP_INTPTR>({_lLoad} > {_rLoad} ? 1 : 0)");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}{{");
                        builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
                        builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
                        EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0)");
                        builder.AppendLine($"{indentation}}}");
                    }
                    PushSlotType(SlotType.NativeInt);
                    break;
                }
            case "clt":
            case "clt.un":
                {
                    SlotType _rType = PeekSlotType();
                    string _rExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    SlotType _lType = PeekSlotType();
                    string _lExpr = ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    string _rLoad = _rType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_rExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_rExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})",
                    };
                    string _lLoad = _lType switch
                    {
                        SlotType.Float32 => $"ChaosLoadFloat32({_lExpr})",
                        SlotType.Float64 => $"ChaosLoadFloat64({_lExpr})",
                        _ => $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})",
                    };
                    if (_activeStructuredSlotContext is not null)
                    {
                        EmitEvalStackPush(builder, indentation,
                            $"static_cast<CHAOS_IL2CPP_INTPTR>({_lLoad} < {_rLoad} ? 1 : 0)");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}{{");
                        builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
                        builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
                        EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left < chaos_right ? 1 : 0)");
                        builder.AppendLine($"{indentation}}}");
                    }
                    PushSlotType(SlotType.NativeInt);
                    break;
                }
            case "add":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapAdd");
                break;
            case "sub":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
                break;
            case "add.ovf":
            case "add.ovf.un":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapAdd");
                break;
            case "sub.ovf":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
                break;
            case "sub.ovf.un":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
                break;
            case "mul.ovf":
            case "mul.ovf.un":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapMul");
                break;
            case "mul":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapMul");
                break;
            case "div":
            case "div.un":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosDiv");
                break;
            case "rem":
            case "rem.un":
                EmitLinearBinaryArithmetic(builder, indentation, "ChaosRem");
                break;
            case "shl":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_left_int32");
                break;
            case "shr":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_int32");
                break;
            case "shr.un":
                EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_un_int32");
                break;
            case "not":
                EmitLinearBitwiseNot(builder, instruction, indentation);
                break;
            case "and":
                EmitLinearBinaryBitwise(builder, indentation, "&");
                break;
            case "or":
                EmitLinearBinaryBitwise(builder, indentation, "|");
                break;
            case "xor":
                EmitLinearBinaryBitwise(builder, indentation, "^");
                break;

            case "ldlen":
                {
                    var arrExpr = ConsumeEvalStackValueExpression();
                    EmitEvalStackPush(builder, indentation,
                        "[&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(" + arrExpr + "); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }()");
                    break;
                }
            case "dup":
                {
                    string source = AccessEvalStackTopExpression();
                    string destination = AllocateEvalStackTargetExpression();
                    builder.AppendLine($"{indentation}{destination} = {source};");
                    break;
                }
            case "neg":
                {
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(-static_cast<CHAOS_IL2CPP_INT32>({AccessEvalStackTopExpression()}));");
                    break;
                }
            case "conv.i4":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.u4":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.i1":
            case "conv.u1":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.i2":
            case "conv.u2":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.i8":
            case "conv.u8":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    EmitEvalStackPush(builder, indentation, $"ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>({_loadExpr}))", SlotType.Int64);
                    PushSlotType(SlotType.Int64);
                    break;
                }
            case "conv.r4":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    EmitEvalStackPush(builder, indentation, $"static_cast<float>({_loadExpr})", SlotType.Float32);
                    PushSlotType(SlotType.Float32);
                    break;
                }
            case "conv.r8":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    ConsumeEvalStackValueExpression();
                    ConsumeSlotType();
                    EmitEvalStackPush(builder, indentation, $"static_cast<double>({_loadExpr})", SlotType.Float64);
                    PushSlotType(SlotType.Float64);
                    break;
                }
            case "conv.u":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.i":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({_loadExpr}));");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "ckfinite":
                {
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_fp = ChaosLoadFloat64({AccessEvalStackTopExpression()});");
                    builder.AppendLine($"{indentation}    if (!std::isfinite(chaos_fp)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.i1":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT8_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.u1":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.i2":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT16_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.u2":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.i4":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT32_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.u4":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.i8":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    // INT64_MIN/MAX check is a no-op on 64-bit platforms but catches
                    // narrowing on 32-bit platforms where INTPTR is 32-bit.
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4");
                    builder.AppendLine($"{indentation}    if (static_cast<CHAOS_IL2CPP_INT64>(chaos_val) < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MIN) || static_cast<CHAOS_IL2CPP_INT64>(chaos_val) > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INT64_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}#endif");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = ChaosStoreInt64(chaos_val);");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u8":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}#if CHAOS_IL2CPP_SIZEOF_VOID_P == 4");
                    // UINT64 range check catches narrowing on 32-bit platforms.
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINT64_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}#endif");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.i":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_INT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INTPTR_MIN) || chaos_val > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_INTPTR_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_val);");
                    builder.AppendLine($"{indentation}}}");
                    UpdateSlotType(SlotType.NativeInt);
                    break;
                }
            case "conv.ovf.i.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INTPTR_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_val);");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u":
                {
                    string _loadExpr = PrepareConvOvfValue();
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr});");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_IL2CPP_UINTPTR_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.i8.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT64_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u8.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr})));");
                    break;
                }
            case "conv.ovf.i1.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.i2.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.i4.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u1.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u2.un":
                {
                    string _loadExpr = PrepareConvOvfValue(useUintptrDefault: true);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u4.un":
                {
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()});");
                    builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "conv.ovf.u.un":
                {
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()}));");
                    break;
                }

            case "conv.r.un":
                {
                    builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreFloat32(static_cast<float>(static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})));");
                    PushSlotType(SlotType.Float32);
                    break;
                }
            case "ldloca":
                {
                    EmitEvalStackPush(builder, indentation, $"&chaos_locals[{GetRequiredIntOperand(instruction)}]");
                    break;
                }
            case "ldarga":
                {
                    EmitEvalStackPush(builder, indentation, $"&chaos_args[{GetRequiredIntOperand(instruction)}]");
                    break;
                }
            case "box":
                {
                    AotCoreIrReferenceArtifact boxTargetRef = GetRequiredTargetReference(instruction);
                    if (boxTargetRef.Kind == AotCoreIrReferenceKind.Type && IsEnumRef(boxTargetRef))
                    {
                        _pendingEnumBoxSubjectId = boxTargetRef.SubjectId;
                        // Skip box emission - raw value stays on eval stack,
                        // consumed by subsequent call/callvirt peephole.
                    }
                    else if (boxTargetRef.Kind == AotCoreIrReferenceKind.Type
                        && ToCharEligiblePrimitives.Contains(ExtractTypeName(boxTargetRef.SubjectId)))
                    {
                        // Pattern 1: box <primitive> + call/callvirt ToChar(Object)
                        if (TryGetLookaheadInstruction(nextInstruction, out var lookaheadInstr)
                            && (lookaheadInstr.Op == "call" || lookaheadInstr.Op == "callvirt")
                            && IsConvertToCharObjectCall(lookaheadInstr))
                        {
                            _pendingBoxSubjectId = ExtractTypeName(boxTargetRef.SubjectId);
                            _pendingBoxHasProvider = false;
                        }
                        // Pattern 2: box <primitive> + ldnull + call/callvirt ToChar(Object, IFormatProvider)
                        else if (TryGetLookaheadInstruction(nextInstruction, out var ldnullInstr)
                            && ldnullInstr.Op == "ldnull"
                            && TryGetSecondLookaheadInstruction(nextInstruction, out var providerCall)
                            && (providerCall.Op == "call" || providerCall.Op == "callvirt")
                            && IsConvertToCharObjectProviderCall(providerCall))
                        {
                            _pendingBoxSubjectId = ExtractTypeName(boxTargetRef.SubjectId);
                            _pendingBoxHasProvider = true;
                            // Skip box emission; ldnull will be processed normally,
                            // and EmitFusedConvertCharBoxCall will pop the provider null.
                        }
                        // Pattern 3: box <primitive> + ldstr + call/callvirt Enum::Format(Type, Object, String)
                        else if (TryGetLookaheadInstruction(nextInstruction, out var ldstrInstr)
                            && ldstrInstr.Op == "ldstr"
                            && TryGetSecondLookaheadInstruction(nextInstruction, out var formatCall)
                            && (formatCall.Op == "call" || formatCall.Op == "callvirt")
                            && IsEnumFormatCall(formatCall))
                        {
                            _pendingBoxSubjectId = ExtractTypeName(boxTargetRef.SubjectId);
                            // Skip box emission; ldstr will be processed normally,
                            // and EmitFusedEnumFormatBoxCall will handle the fused call.
                        }
                        else
                        {
                            EmitLinearBox(builder, instruction, indentation);
                        }
                    }
                    else
                    {
                        EmitLinearBox(builder, instruction, indentation);
                    }
                    break;
                }
            case "unbox":
                EmitLinearUnbox(builder, instruction, indentation);
                break;
            case "unbox.any":
                EmitLinearUnboxAny(builder, instruction, indentation);
                break;
            case "castclass":
                EmitLinearCastClass(builder, instruction, indentation);
                break;
            case "isinst":
                EmitLinearIsInst(builder, instruction, indentation);
                break;
            case "initobj":
                ResetArrayCheckCache();
                EmitLinearInitObj(builder, instruction, indentation);
                break;
            case "newarr":
                EmitLinearNewArray(builder, instruction, indentation);
                break;
            case "ldelema":
                EmitLinearArrayElementAddress(builder, instruction, indentation);
                break;
            case "sizeof":
                {
                    var targetRef = GetRequiredTargetReference(instruction);
                    string sizeExpr = targetRef.SubjectId switch
                    {
                        "System.Byte" or "System.SByte" or "System.Boolean" => "1",
                        "System.Int16" or "System.UInt16" or "System.Char" => "2",
                        "System.Int32" or "System.UInt32" or "System.Single" => "4",
                        "System.Int64" or "System.UInt64" or "System.Double" => "8",
                        "System.IntPtr" or "System.UIntPtr" => "sizeof(void*)",
                        _ => "4"
                    };
                    EmitEvalStackPush(builder, indentation, $"static_cast<CHAOS_IL2CPP_INTPTR>({sizeExpr})");
                }
                break;
            case "ldind.i1":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.u1":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT8", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.i2":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.u2":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT16", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.i4":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.u4":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT32", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.i8":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>(chaos_value)", indentation);
                break;
            case "ldind.r4":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "ChaosStoreFloat32(chaos_value)", indentation);
                PushSlotType(SlotType.Float32);
                break;
            case "ldind.r8":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "ChaosStoreFloat64(chaos_value)", indentation);
                PushSlotType(SlotType.Float64);
                break;
            case "ldind.ref":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "ldind.i":
                EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
                break;
            case "stind.i1":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.i2":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.i4":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.i8":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.r4":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.r8":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "stind.ref":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value_raw)", indentation, materializeString: true, needsSatbBarrier: true);
                break;
            case "stind.i":
                EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value_raw)", indentation, materializeString: false);
                break;
            case "ldflda":
            case "ldsflda":
                EmitLinearFieldAddress(builder, instruction, indentation);
                break;
            case "stsfld":
                {
                    var targetRef = GetRequiredTargetReference(instruction);
                    if (targetRef.Kind != AotCoreIrReferenceKind.Field)
                    {
                        throw new NotSupportedException($"native-aot structured EH linear field store requires field target reference, got '{targetRef.Kind}'.");
                    }
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
                    EmitStaticInitializationForField(builder, targetRef.SubjectId, indentation);
                    string fieldSymbol = GetNativeStaticFieldSymbol(targetRef.SubjectId);
                    // Root change barrier: record old value before overwriting, so BGC
                    // re-mark can re-scan roots that were already scanned.  Skip for
                    // primitive types (they never hold GC references).
                    string? fieldTypeId = targetRef.FieldTypeSubjectId;
                    if (fieldTypeId == null || !(PrimitiveValueTypeSubjectIds.Contains(fieldTypeId) || PrimitiveValueTypeSubjectIds.Contains("System.Private.CoreLib/" + fieldTypeId)))
                    {
                        builder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&{fieldSymbol}));");
                        builder.AppendLine($"{indentation}    BgcRecordRootChange(reinterpret_cast<void**>(&{fieldSymbol}), reinterpret_cast<void*>({fieldSymbol}));");
                    }
                    builder.AppendLine($"{indentation}    {fieldSymbol} = chaos_value;");
                    builder.AppendLine($"{indentation}}}");
                    if (_staticFieldDeclarations == null)
                        _staticFieldDeclarations = new Dictionary<string, string?>(System.StringComparer.Ordinal);
                    _staticFieldDeclarations.TryAdd(targetRef.SubjectId, targetRef.FieldTypeSubjectId);
                    break;
                }
            case "stfld":
                {
                    var targetRef = GetRequiredTargetReference(instruction);
                    if (targetRef.Kind != AotCoreIrReferenceKind.Field)
                    {
                        throw new NotSupportedException($"native-aot structured EH linear field store requires field target reference, got '{targetRef.Kind}'.");
                    }

                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
                    if (_stringIdMapping is { Count: > 0 })
                    {
                        builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->is_string_id(chaos_value))");
                        builder.AppendLine($"{indentation}    {{");
                        builder.AppendLine($"{indentation}        chaos_value = chaos_runtime_get_abi_v0()->string_materialize(chaos_value);");
                        builder.AppendLine($"{indentation}    }}");
                    }

                    string declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetRef);
                    if (targetRef.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
                    {
                        builder.AppendLine($"{indentation}    auto* chaos_value_owner = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
                        builder.AppendLine($"{indentation}    chaos_value_owner->{GetNativeFieldMemberName(targetRef.SubjectId)} = chaos_value;");
                        // Skip write barrier for primitive field types — they never hold GC references
                        string? fieldTypeId = targetRef.FieldTypeSubjectId;
                        if (fieldTypeId == null || !(PrimitiveValueTypeSubjectIds.Contains(fieldTypeId) || PrimitiveValueTypeSubjectIds.Contains("System.Private.CoreLib/" + fieldTypeId)))
                        {
                            builder.AppendLine($"{indentation}    if (chaos_is_gc_pointer(chaos_value_owner))");
                            builder.AppendLine($"{indentation}    {{");
                            builder.AppendLine($"{indentation}        chaos_gc_dirty_card(chaos_value_owner);");
                            builder.AppendLine($"{indentation}    }}");
                        }
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>({ConsumeEvalStackValueExpression()});");
                        // Skip write barrier for primitive field types — they never hold GC references
                        string? fieldTypeId = targetRef.FieldTypeSubjectId;
                        if (fieldTypeId == null || !(PrimitiveValueTypeSubjectIds.Contains(fieldTypeId) || PrimitiveValueTypeSubjectIds.Contains("System.Private.CoreLib/" + fieldTypeId)))
                        {
                            builder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->{GetNativeFieldMemberName(targetRef.SubjectId)}));");
                        }
                        builder.AppendLine($"{indentation}    chaos_object->{GetNativeFieldMemberName(targetRef.SubjectId)} = chaos_value;");
                        if (fieldTypeId == null || !(PrimitiveValueTypeSubjectIds.Contains(fieldTypeId) || PrimitiveValueTypeSubjectIds.Contains("System.Private.CoreLib/" + fieldTypeId)))
                        {
                            builder.AppendLine($"{indentation}    chaos_gc_dirty_card(chaos_object);");
                        }
                    }

                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "cpobj":
                EmitLinearCopyObject(builder, instruction, indentation);
                break;
            case "cpblk":
                EmitLinearCopyBlock(builder, indentation);
                break;
            case "ldelem.i1":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT8");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.u1":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT8");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.i2":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT16");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.u2":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT16");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.i4":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT32");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.i":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INTPTR");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.u4":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT32");
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem.i8":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INT64>(chaos_element)", indentation);
                PushSlotType(SlotType.Int64);
                break;
            case "ldelem.r4":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "ChaosStoreFloat32(ChaosLoadFloat32(chaos_element))", indentation);
                PushSlotType(SlotType.Float32);
                break;
            case "ldelem.r8":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "ChaosStoreFloat64(ChaosLoadFloat64(chaos_element))", indentation);
                PushSlotType(SlotType.Float64);
                break;
            case "ldelem.ref":
                ConsumeSlotType();
                ConsumeSlotType();
                EmitLinearArrayLoad(builder, instruction, "chaos_element", indentation);
                PushSlotType(SlotType.NativeInt);
                break;
            case "ldelem":
                {
                    var targetRef = instruction.TargetReference;
                    string? subjectId = null;
                    AotCoreIrTypeShapeKind typeShape = AotCoreIrTypeShapeKind.ValueType;

                    if (targetRef != null && targetRef.Kind == AotCoreIrReferenceKind.Type)
                    {
                        subjectId = HasArrayElementReference(targetRef)
                            ? targetRef.ArrayElementSubjectId!
                            : targetRef.SubjectId;
                        typeShape = HasArrayElementReference(targetRef)
                            ? targetRef.ArrayElementTypeShape
                            : targetRef.TypeShape;
                    }
                    else if (instruction.Operand is string operandTypeName)
                    {
                        // Generic ldelem with Operand as type name (no TargetReference)
                        subjectId = operandTypeName;
                    }

                    if (subjectId == null
                        || typeShape == AotCoreIrTypeShapeKind.ReferenceType
                        || typeShape == AotCoreIrTypeShapeKind.InterfaceType)
                    {
                        EmitLinearArrayLoad(builder, instruction, "chaos_element", indentation);
                    }
                    else
                        switch (subjectId)
                        {
                            case "System.Byte":
                            case "System.SByte":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT8");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.Boolean":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT8");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.Int16":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT16");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.UInt16":
                            case "System.Char":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT16");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.Int32":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_INT32");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.UInt32":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element)", indentation, elementType: "CHAOS_IL2CPP_UINT32");
                                PushSlotType(SlotType.NativeInt);
                                break;
                            case "System.Int64":
                            case "System.UInt64":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "static_cast<CHAOS_IL2CPP_INT64>(chaos_element)", indentation);
                                PushSlotType(SlotType.Int64);
                                break;
                            case "System.Single":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "ChaosStoreFloat32(ChaosLoadFloat32(chaos_element))", indentation);
                                PushSlotType(SlotType.Float32);
                                break;
                            case "System.Double":
                                ConsumeSlotType();
                                ConsumeSlotType();
                                EmitLinearArrayLoad(builder, instruction, "ChaosStoreFloat64(ChaosLoadFloat64(chaos_element))", indentation);
                                PushSlotType(SlotType.Float64);
                                break;
                            default:
                                EmitLinearArrayLoad(builder, instruction, "chaos_element", indentation); break;
                        }
                    break;
                }
            case "stelem.i1":
                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT8");
                break;
            case "stelem.i2":
                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT16");
                break;
            case "stelem.i4":
                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT32");
                break;
            case "stelem.i8":
                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, isReferenceElement: false);
                break;
            case "stelem.r4":
                EmitLinearArrayStore(builder, instruction, "ChaosStoreFloat32(static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw))", indentation, isReferenceElement: false);
                break;
            case "stelem.r8":
                EmitLinearArrayStore(builder, instruction, "ChaosStoreFloat64(static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw))", indentation, isReferenceElement: false);
                break;
            case "stelem":
                {
                    var targetRef = instruction.TargetReference;
                    string? subjectId = null;
                    AotCoreIrTypeShapeKind typeShape = AotCoreIrTypeShapeKind.ValueType;

                    if (targetRef != null && targetRef.Kind == AotCoreIrReferenceKind.Type)
                    {
                        subjectId = HasArrayElementReference(targetRef)
                            ? targetRef.ArrayElementSubjectId!
                            : targetRef.SubjectId;
                        typeShape = HasArrayElementReference(targetRef)
                            ? targetRef.ArrayElementTypeShape
                            : targetRef.TypeShape;
                    }
                    else if (instruction.Operand is string operandTypeName)
                    {
                        // Generic stelem with Operand as type name (no TargetReference)
                        subjectId = operandTypeName;
                    }

                    if (subjectId == null
                        || typeShape == AotCoreIrTypeShapeKind.ReferenceType
                        || typeShape == AotCoreIrTypeShapeKind.InterfaceType)
                    {
                        EmitLinearArrayStore(builder, instruction, "chaos_value_raw", indentation, isReferenceElement: false);
                    }
                    else
                        switch (subjectId)
                        {
                            case "System.Byte":
                            case "System.SByte":
                            case "System.Boolean":
                                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT8"); break;
                            case "System.Int16":
                            case "System.UInt16":
                            case "System.Char":
                                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT16"); break;
                            case "System.Int32":
                            case "System.UInt32":
                                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw)", indentation, isReferenceElement: false, elementType: "CHAOS_IL2CPP_INT32"); break;
                            case "System.Int64":
                            case "System.UInt64":
                                EmitLinearArrayStore(builder, instruction, "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, isReferenceElement: false); break;
                            case "System.Single":
                                EmitLinearArrayStore(builder, instruction, "ChaosStoreFloat32(static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw))", indentation, isReferenceElement: false); break;
                            case "System.Double":
                                EmitLinearArrayStore(builder, instruction, "ChaosStoreFloat64(static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw))", indentation, isReferenceElement: false); break;
                            default:
                                EmitLinearArrayStore(builder, instruction, "chaos_value_raw", indentation, isReferenceElement: false); break;
                        }
                    break;
                }

            case "ldobj":
                EmitLinearLoadObjectValue(builder, instruction, indentation);
                break;
            case "stobj":
                EmitLinearStoreObjectValue(builder, instruction, indentation);
                break;
            case "localloc":
                EmitLinearLocalAlloc(builder, indentation);
                break;
            case "initblk":
                {
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_count = static_cast<CHAOS_IL2CPP_SIZE>({ConsumeEvalStackValueExpression()});");
                    builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_UINT8>({ConsumeEvalStackValueExpression()});");
                    builder.AppendLine($"{indentation}    auto* chaos_addr = reinterpret_cast<void*>({ConsumeEvalStackValueExpression()});");
                    builder.AppendLine($"{indentation}    memset(chaos_addr, chaos_value, chaos_count);");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "switch":
                {
                    builder.AppendLine($"{indentation}// switch (handled via terminator in structured IR)");
                    break;
                }
            case "arglist":
                {
                    EmitEvalStackPush(builder, indentation, "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args)");
                    break;
                }
            case "mkrefany":
                {
                    string chaosTypeHandle = GetRequiredTypeHandleLiteral(instruction);
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_ptr = {ConsumeEvalStackValueExpression()};");
                    EmitEvalStackPush(builder, indentation + "    ", chaosTypeHandle);
                    EmitEvalStackPush(builder, indentation + "    ", "chaos_ptr");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "refanyval":
                {
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_ptr = {ConsumeEvalStackValueExpression()};");
                    builder.AppendLine($"{indentation}    static_cast<void>({ConsumeEvalStackValueExpression()});");
                    EmitEvalStackPush(builder, indentation + "    ", "chaos_ptr");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "refanytype":
                {
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    static_cast<void>({ConsumeEvalStackValueExpression()});");
                    builder.AppendLine($"{indentation}    const auto chaos_typeHandle = {ConsumeEvalStackValueExpression()};");
                    EmitEvalStackPush(builder, indentation + "    ", "chaos_typeHandle");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "ldvirtftn":
                {
                    var targetRef = GetRequiredTargetReference(instruction);
                    if (targetRef.Kind != AotCoreIrReferenceKind.Method)
                    {
                        throw new NotSupportedException($"native-aot structured EH linear ldvirtftn requires method target reference, got '{targetRef.Kind}'.");
                    }
                    string vtableSlotSig = GetMethodSignatureSuffix(targetRef.SubjectId);
                    if (_vtableSlotMap == null || !_vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
                    {
                        throw new NotSupportedException($"native-aot structured EH linear ldvirtftn: vtable slot not found for '{targetRef.SubjectId}'.");
                    }
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<chaos_managed_object*>({ConsumeEvalStackValueExpression()});");
                    builder.AppendLine($"{indentation}    if (chaos_object == nullptr) {{ CHAOS_IL2CPP_FAIL(); }}");
                    builder.AppendLine($"{indentation}    auto* chaos_type = chaos_object_get_type_info(chaos_object);");
                    builder.AppendLine($"{indentation}    auto chaos_fn = chaos_type->vtable_array[{vtableSlot}u];");
                    EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn)");
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "calli":
                {
                    var calliParamAbis = CreateCallSiteParameterAbis(instruction);
                    var calliReturnAbi = CreateCallSiteReturnAbi(instruction);
                    string calliReturnType = MapAbiSlotReturnType(calliReturnAbi);
                    string calliParamTypes = FormatAbiSlotParameterTypes(calliParamAbis);
                    string calliFnType = $"{calliReturnType}(*)({calliParamTypes})";
                    builder.AppendLine($"{indentation}{{");
                    builder.AppendLine($"{indentation}    const auto chaos_fnptr = {ConsumeEvalStackValueExpression()};");
                    for (int calliIdx = calliParamAbis.Count - 1; calliIdx >= 0; calliIdx--)
                    {
                        builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{calliIdx} = {ConsumeEvalStackValueExpression()};");
                        builder.AppendLine($"{indentation}    const auto chaos_arg_{calliIdx} = {FormatInboundAbiArgumentExpression(calliParamAbis[calliIdx], $"chaos_raw_arg_{calliIdx}")};");
                    }
                    string calliArgs = FormatAbiInvocationArgumentList(calliParamAbis);
                    if (string.Equals(calliReturnType, "void", StringComparison.Ordinal))
                    {
                        builder.AppendLine($"{indentation}    reinterpret_cast<{calliFnType}>(chaos_fnptr)({calliArgs});");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}    const auto chaos_result = reinterpret_cast<{calliFnType}>(chaos_fnptr)({calliArgs});");
                        EmitAbiReturnPush(builder, calliReturnAbi, "chaos_result", $"{indentation}    ");
                    }
                    builder.AppendLine($"{indentation}}}");
                    break;
                }
            case "jmp":
                {
                    var jmpTarget = ResolveDirectInvocationTarget(instruction);
                    var jmpParamAbis = jmpTarget.ParameterAbis;
                    if (jmpParamAbis.Count == 0)
                    {
                        builder.AppendLine($"{indentation}return {jmpTarget.TargetSymbol}();");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}{{");
                        for (int jmpIdx = 0; jmpIdx < jmpParamAbis.Count; jmpIdx++)
                        {
                            builder.AppendLine($"{indentation}    const auto chaos_jmp_arg_{jmpIdx} = {FormatInboundAbiArgumentExpression(jmpParamAbis[jmpIdx], $"chaos_args[{jmpIdx}]")};");
                        }
                        string jmpArgs = FormatAbiInvocationArgumentList(jmpParamAbis);
                        builder.AppendLine($"{indentation}    return {jmpTarget.TargetSymbol}({jmpArgs});");
                        builder.AppendLine($"{indentation}}}");
                    }
                    break;
                }
            case "ret":
                {
                    builder.AppendLine($"{indentation}// ret (handled via terminator in structured IR)");
                    break;
                }
            case "throw":
                {
                    string throwVal = ConsumeEvalStackValueExpression();
                    EmitThrowCpp(builder, throwVal, indentation);
                    break;
                }
            case "rethrow":
                {
                    EmitRethrowCpp(builder, indentation);
                    break;
                }
            case "break":
                {
                    builder.AppendLine("#if defined(_MSC_VER)");
                    builder.AppendLine(indentation + "    __debugbreak();");
                    builder.AppendLine("#else");
                    builder.AppendLine(indentation + "    __builtin_trap();");
                    builder.AppendLine("#endif");
                    break;
                }
            case "nop":
                {
                    break;
                }
            case "beq":
            case "bge":
            case "bge.un":
            case "bgt":
            case "bgt.un":
            case "ble":
            case "ble.un":
            case "blt":
            case "blt.un":
            case "bne.un":
            case "brfalse":
            case "brtrue":
                {
                    builder.AppendLine($"{indentation}// {instruction.Op} (structured EH branch)");
                    break;
                }
            case "endcatch":
                {
                    builder.AppendLine($"{indentation}// endcatch (handled via structured EH)");
                    break;
                }
            case "br.s":
            case "br":
            case "leave":
                {
                    builder.AppendLine($"{indentation}// {instruction.Op} (handled via structured EH branches)");
                    break;
                }
            case "endfinally":
                {
                    builder.AppendLine($"{indentation}// endfinally (handled via structured EH)");
                    break;
                }
            case "endfilter":
                {
                    builder.AppendLine($"{indentation}// endfilter (handled via structured EH)");
                    break;
                }
            default:
                throw new NotSupportedException("native-aot structured EH linear lowering does not support opcode '" + instruction.Op + "'.");
        }
    }





    private string PrepareConvOvfValue(bool useUintptrDefault = false)
    {
        SlotType _slotType = PeekSlotType();
        // In structured mode, _fN/_dN slots are already float/double C++ variables.
        // The ChaosLoadFloat* wrappers would do double->intptr truncation -> memcpy = garbage.
        if (_activeStructuredSlotContext is not null && _slotType is SlotType.Float32 or SlotType.Float64)
            return AccessEvalStackTopExpression();
        // In structured mode, _iN slots are already int64_t C++ variables.
        if (_activeStructuredSlotContext is not null && _slotType is SlotType.Int64)
        {
            var expr = AccessEvalStackTopExpression();
            if (expr.StartsWith("_i", StringComparison.Ordinal))
                return expr;
            return $"ChaosLoadInt64({expr})";
        }
        return _slotType switch
        {
            SlotType.Float32 => $"ChaosLoadFloat32({AccessEvalStackTopExpression()})",
            SlotType.Float64 => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
            SlotType.Int64 => $"ChaosLoadInt64({AccessEvalStackTopExpression()})",
            _ => useUintptrDefault
                ? $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})"
                : AccessEvalStackTopExpression(),
        };
    }






    /// <summary>
    /// Scan backward from current instruction to find the target method for Assert.Throws.
    /// Looks for the pattern: ldnull + ldftn <method> + newobj Action::.ctor.
    /// Returns (symbol, isStatic) — isStatic indicates whether the target method is
    /// static (no implicit 'this' argument) vs. an instance method on a DisplayClass.
    /// </summary>
    private (string? Symbol, bool IsStatic) FindThrowsTargetMethod()
    {
        var list = _linearInstructionList ?? _lookaheadInstructionList;
        var idx = _linearInstructionList != null ? _linearInstructionIndex : _lookaheadInstructionIndex;
        if (list == null || idx < 0) return (null, true);

        // Scan backward from idx-1 to find newobj Action::.ctor preceded by ldftn
        for (int i = idx - 1; i >= 0; i--)
        {
            var instr = list[i];
            if (instr.OpCode == InstructionOpCode.NewObj &&
                instr.Callee != null &&
                instr.Callee.Contains("System.Action::.ctor"))
            {
                // Found newobj. The ldftn should be at i-1
                if (i >= 1 && list[i - 1].OpCode == InstructionOpCode.LdFtn)
                {
                    var ldftnInstr = list[i - 1];
                    if (ldftnInstr.Callee != null &&
                        _methodsBySubjectId.TryGetValue(ldftnInstr.Callee, out var targetMethod))
                    {
                        return (TryGetInstantiationStubSymbol(targetMethod) ?? targetMethod.NativeSymbol,
                            // DisplayClass b__0 methods (lambda closures) always take the
                            // delegate target as their first CHAOS_IL2CPP_INTPTR argument,
                            // even when C# emits them as static (no-capture lambdas).
                            // Override IsStatic to false so the caller passes the target.
                            targetMethod.IsStatic &&
                            !(targetMethod.SubjectId.Contains("DisplayClass") &&
                              targetMethod.SubjectId.Contains("_b__")));
                    }
                    if (!string.IsNullOrEmpty(ldftnInstr.TargetSymbol))
                    {
                        // Lambda closure methods (DisplayClass + b__0 pattern) are instance
                        // methods — they take the delegate target as their first argument.
                        // Detect this pattern from the Callee SubjectId when the method
                        // metadata is unavailable via _methodsBySubjectId.
                        var callee = ldftnInstr.Callee ?? "";
                        bool isLambdaClosure = callee.Contains("DisplayClass") &&
                            callee.Contains("_b__");
                        return (ldftnInstr.TargetSymbol, !isLambdaClosure);
                    }
                }
            }
        }
        return (null, true);
    }





    /// <summary>
    /// Try to inline a resolved callee method at the call site.
    /// Used by both direct calls and devirtualized callvirt.
    /// Supports simple multi-BB callees (up to 3 basic blocks) with branch
    /// target remapping and unified exit via goto chaos_inline_end{N}.
    /// </summary>
    private bool TryInlineResolvedMethod(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string calleeSubjectId, int paramCount, string indentation, int depth = 1)
    {
        if (!_methodsBySubjectId.TryGetValue(calleeSubjectId, out var calleeMethod)) return false;
        if (calleeMethod.ExceptionRegionCount > 0) return false;
        if (calleeMethod.IsPInvoke) return false;
        if (_currentMethodArtifact == null) return false;
        if (_currentMethodNativeSymbol == null) return false;
        if (depth > InliningPlanner.kMaxInlineDepth) return false;
        int inlineId = _nextInlineId++;

        // Budget check via InliningPlanner
        bool isRecursive = string.Equals(calleeMethod.NativeSymbol, _currentMethodNativeSymbol, StringComparison.Ordinal);
        try
        {
            var candidate = InliningPlanner.EvaluateInline(calleeMethod.Instructions.Count, _currentMethodArtifact.Instructions.Count, isRecursive);
            if (!candidate.CanInline) return false;

            // Multi-BB support: scan callee instructions to find branch targets and count basic blocks.
            var branchTargets = new HashSet<int>();
            int basicBlockCount = 1;
            foreach (var ci in calleeMethod.Instructions)
            {
                switch (ci.Op)
                {
                    case "br.s":
                    case "br":
                    case "brfalse.s":
                    case "brfalse":
                    case "brtrue.s":
                    case "brtrue":
                    case "beq.s":
                    case "beq":
                    case "bne.un.s":
                    case "bne.un":
                    case "bge.s":
                    case "bge":
                    case "bge.un.s":
                    case "bge.un":
                    case "bgt.s":
                    case "bgt":
                    case "bgt.un.s":
                    case "bgt.un":
                    case "ble.s":
                    case "ble":
                    case "ble.un.s":
                    case "ble.un":
                    case "blt.s":
                    case "blt":
                    case "blt.un.s":
                    case "blt.un":
                        branchTargets.Add(GetRequiredIntOperand(ci));
                        basicBlockCount++;
                        break;
                    case "switch":
                    case "leave":
                    case "endfilter":
                    case "endfinally":
                    case "starg":
                    case "ldarga":
                    case "ldloca":
                        return false;
                }
            }

            if (basicBlockCount > 3) return false;

            // Build IlOffset to label number mapping for branch targets
            var labelMap = new Dictionary<int, int>();
            int labelIdx = 0;
            foreach (int target in branchTargets.OrderBy(t => t))
            {
                labelMap[target] = labelIdx++;
            }

            // Determine if callee is non-void (has a return value on eval stack)
            bool calleeHasReturn = !string.Equals(calleeMethod.ReturnType, "System.Void", StringComparison.Ordinal)
                                   && !string.Equals(calleeMethod.ReturnType, "void", StringComparison.Ordinal);

            // ---- EMIT INLINE BODY ----
            builder.AppendLine($"{indentation}{{");
            builder.AppendLine($"{indentation}    // Inlined: {calleeMethod.SubjectId}");

            // Consume arguments from eval stack into local C++ variables
            for (int i = paramCount - 1; i >= 0; i--)
            {
                string argExpr = ConsumeEvalStackValueExpression();
                builder.AppendLine($"{indentation}    auto chaos_inline_arg_{i} = {argExpr};");
            }

            // For non-void callees, capture return value at each ret and push at unified exit
            if (calleeHasReturn)
            {
                builder.AppendLine($"{indentation}    CHAOS_IL2CPP_INTPTR chaos_inline_retval{inlineId}{{}};");
            }

            int localOffset = _currentMethodArtifact.LocalCount;

            foreach (var calleeInstruction in calleeMethod.Instructions)
            {
                // Emit label if this instruction is a branch target
                if (labelMap.TryGetValue(calleeInstruction.IlOffset, out int lbl))
                {
                    builder.AppendLine($"{indentation}chaos_inline_label_{inlineId}_{lbl}:");
                    builder.AppendLine($"{indentation}{{");
                }

                switch (calleeInstruction.Op)
                {
                    case "ret":
                        {
                            if (calleeHasReturn)
                            {
                                string retVal = ConsumeEvalStackValueExpression();
                                builder.AppendLine($"{indentation}    chaos_inline_retval{inlineId} = {retVal};");
                            }
                            builder.AppendLine($"{indentation}    goto chaos_inline_end{inlineId};");
                            break;
                        }

                    case "ldarg":
                        {
                            int argIndex = GetRequiredIntOperand(calleeInstruction);
                            EmitEvalStackPush(builder, indentation + "    ", $"chaos_inline_arg_{argIndex}");
                            break;
                        }

                    case "ldloc":
                        {
                            int localIndex = GetRequiredIntOperand(calleeInstruction);
                            EmitEvalStackPush(builder, indentation + "    ", $"chaos_locals[{localOffset + localIndex}]");
                            break;
                        }

                    case "stloc":
                        {
                            string valueExpr = ConsumeEvalStackValueExpression();
                            int localIndex = GetRequiredIntOperand(calleeInstruction);
                            builder.AppendLine($"{indentation}    chaos_locals[{localOffset + localIndex}] = {valueExpr};");
                            break;
                        }

                    case "br.s":
                    case "br":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (labelMap.TryGetValue(targetOff, out int brLbl))
                                builder.AppendLine($"{indentation}    goto chaos_inline_label_{inlineId}_{brLbl};");
                            break;
                        }

                    case "brfalse":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int bfLbl)) break;
                            string condVal = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({condVal} == 0) goto chaos_inline_label_{inlineId}_{bfLbl};");
                            break;
                        }

                    case "brtrue":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int btLbl)) break;
                            string condVal = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({condVal} != 0) goto chaos_inline_label_{inlineId}_{btLbl};");
                            break;
                        }

                    case "beq":
                    case "beq.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int eqLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} == {r}) goto chaos_inline_label_{inlineId}_{eqLbl};");
                            break;
                        }

                    case "bne.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int neLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} != {r}) goto chaos_inline_label_{inlineId}_{neLbl};");
                            break;
                        }

                    case "bge":
                    case "bge.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int geLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} >= {r}) goto chaos_inline_label_{inlineId}_{geLbl};");
                            break;
                        }

                    case "bgt":
                    case "bgt.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int gtLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} > {r}) goto chaos_inline_label_{inlineId}_{gtLbl};");
                            break;
                        }

                    case "ble":
                    case "ble.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int leLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} <= {r}) goto chaos_inline_label_{inlineId}_{leLbl};");
                            break;
                        }

                    case "blt":
                    case "blt.un":
                        {
                            int targetOff = GetRequiredIntOperand(calleeInstruction);
                            if (!labelMap.TryGetValue(targetOff, out int ltLbl)) break;
                            string r = ConsumeEvalStackValueExpression();
                            string l = ConsumeEvalStackValueExpression();
                            builder.AppendLine($"{indentation}    if ({l} < {r}) goto chaos_inline_label_{inlineId}_{ltLbl};");
                            break;
                        }

                    default:
                        {
                            EmitInstruction(builder, calleeInstruction, indentation + "    ");
                            break;
                        }
                }

                if (labelMap.ContainsKey(calleeInstruction.IlOffset))
                {
                    builder.AppendLine($"{indentation}}}");
                }
            }

            builder.AppendLine($"{indentation}chaos_inline_end{inlineId}:");
            builder.AppendLine($"{indentation}    ;");
            if (calleeHasReturn)
            {
                EmitEvalStackPush(builder, indentation + "    ", $"chaos_inline_retval{inlineId}");
            }
            builder.AppendLine($"{indentation}}}");
            return true;
        }
        catch (Exception ex) when (ex is InvalidOperationException or NotSupportedException or NullReferenceException)
        {
            // Inlining failed — the callee method has unsupported IL patterns
            // (e.g., structured slot underflow, newobj without metadata) or the
            // inline depth exceeded what the structured emitter can handle.
            // Fall through to non-inlined dispatch.
            return false;
        }
    }





    /// <summary>Try to inline the callee method identified by the invocation target.</summary>
    private bool TryInlineAtCallSite(StringBuilder builder, AotCoreIrInstructionArtifact instruction, InvocationTarget invocationTarget, string indentation)
    {
        string? calleeSubjectId = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        // Fallback: for lowering-time devirtualized calls, the instruction-level
        // Callee/TargetReference is consumed during devirtualization.  Resolve the
        // SubjectId from the native symbol via the reverse symbol table.
        if (calleeSubjectId == null && invocationTarget.TargetSymbol != null)
        {
            bool found = _nativeSymbolToSubjectId.TryGetValue(invocationTarget.TargetSymbol, out var resolvedId);
            if (found)
            {
                calleeSubjectId = resolvedId;
            }
        }
        if (calleeSubjectId == null) return false;
        return TryInlineResolvedMethod(builder, instruction, calleeSubjectId, invocationTarget.ParameterAbis.Count, indentation);
    }

}
