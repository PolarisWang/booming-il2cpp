using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

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
                ? (_state.Value!.ActiveStructuredSlotContext is not null && (_rExpr.StartsWith("_d", StringComparison.Ordinal) || _rExpr.StartsWith("_f", StringComparison.Ordinal))
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
                ? (_state.Value!.ActiveStructuredSlotContext is not null && (_lExpr.StartsWith("_d", StringComparison.Ordinal) || _lExpr.StartsWith("_f", StringComparison.Ordinal))
                    ? _lExpr
                    : $"ChaosLoadFloat64({_lExpr})")
                : $"static_cast<double>({_lExpr})")
            : isInt64Op
                ? (_lIsInt64
                    ? $"ChaosLoadInt64({_lExpr})"
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_lExpr}))")
                : $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})";

        if (_state.Value!.ActiveStructuredSlotContext is not null)
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
                    ? (_state.Value!.ActiveStructuredSlotContext is not null && _rExpr.StartsWith("_i", StringComparison.Ordinal)
                        ? _rExpr
                        : $"ChaosLoadInt64({_rExpr})")
                    : $"static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_INT32>({_rExpr}))";
                string _lLoadInt64 = _lIsInt64
                    ? (_state.Value!.ActiveStructuredSlotContext is not null && _lExpr.StartsWith("_i", StringComparison.Ordinal)
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
                if (_state.Value!.ActiveStructuredSlotContext is not null)
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
        if (_state.Value!.ActiveStructuredSlotContext is not null)
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
        if (_state.Value!.ActiveStructuredSlotContext is not null)
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
        if (_state.Value!.ActiveStructuredSlotContext is not null)
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
        if (_state.Value!.PendingBoxHasProvider)
        {
            ConsumeEvalStackValueExpression();
            _state.Value!.PendingBoxHasProvider = false;
        }
        string rawValueExpr = ConsumeEvalStackValueExpression();
        string inlineExpr = _state.Value!.PendingBoxSubjectId switch
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
            string nativeFn = GetConvertCharNativeFunctionName(_state.Value!.PendingBoxSubjectId!);
            builder.AppendLine(indentation + "{");
            builder.AppendLine(indentation + "    const auto chaos_result = " + nativeFn + "(" + rawValueExpr + ");");
            EmitEvalStackPush(builder, indentation + "    ", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result)");
            builder.AppendLine(indentation + "}");
        }
        _state.Value!.PendingBoxSubjectId = null;
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
