using System;
using System.Collections;
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
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private bool IsTrackedValueTypeSubjectId(string subjectId)
    {
        if (!string.IsNullOrEmpty(subjectId))
        {
            return _valueTypeSubjectIds.Contains(subjectId);
        }
        return false;
    }

    private string GetVirtualDispatchTargetTypeIdSymbol(string subjectId)
    {
        if (!IsTrackedValueTypeSubjectId(subjectId))
        {
            return GetNativeTypeIdSymbol(subjectId);
        }
        return GetNativeBoxTypeIdSymbol(subjectId);
    }

    private string GetVirtualDispatchTargetTypeInfoPointer(string subjectId)
    {
        if (!IsTrackedValueTypeSubjectId(subjectId))
        {
            return GetNativeTypeInfoSymbol(subjectId);
        }
        return GetNativeBoxTypeInfoSymbol(subjectId);
    }

    private string GetVirtualDispatchInstanceExpression(string subjectId, string instanceExpression)
    {
        if (!IsTrackedValueTypeSubjectId(subjectId))
        {
            return instanceExpression;
        }
        string nativeBoxTypeSymbol = GetNativeBoxTypeSymbol(subjectId);
        if (!IsStructuredValueTypeSubjectId(subjectId))
        {
            return $"reinterpret_cast<{nativeBoxTypeSymbol}*>({instanceExpression})->value";
        }
        return $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reinterpret_cast<{nativeBoxTypeSymbol}*>({instanceExpression})->value)";
    }

    private string GetRequiredFunctionPointerTargetSymbol(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? value))
        {
            return TryGetInstantiationStubSymbol(value) ?? value.NativeSymbol;
        }
        if (!string.IsNullOrEmpty(instruction.TargetSymbol))
        {
            return instruction.TargetSymbol;
        }
        throw new NotSupportedException("native-aot lowering does not support unresolved function pointer target '" + (instruction.Callee ?? "<null>") + "'.");
    }

    private static string GetRequiredTargetSymbol(AotCoreIrInstructionArtifact instruction)
    {
        if (TryGetInstantiationStubSymbol(instruction.TargetReference?.InstantiationStubId) is { } text)
        {
            return text;
        }
        if (!string.IsNullOrEmpty(instruction.TargetSymbol))
        {
            return instruction.TargetSymbol;
        }
        throw new NotSupportedException("native-aot lowering does not support unresolved call target '" + (instruction.Callee ?? "<null>") + "'");
    }

    private static int GetRequiredTargetParameterCount(AotCoreIrInstructionArtifact instruction)
    {
        int? targetParameterCount = instruction.TargetParameterCount;
        if (targetParameterCount.HasValue)
        {
            int valueOrDefault = targetParameterCount.GetValueOrDefault();
            if (valueOrDefault >= 0)
            {
                return valueOrDefault;
            }
        }
        // Fallback: infer parameter count from the method signature when metadata is missing.
        // This handles external method calls (e.g. property getters, BCL methods) where
        // targetParameterCount was not populated during IR generation.
        string? callee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (!string.IsNullOrEmpty(callee))
        {
            int inferred = InferParameterCountFromSubjectId(callee);
            if (inferred >= 0)
                return inferred;
        }
        throw new NotSupportedException("native-aot lowering does not support call target '" + (instruction.TargetSymbol ?? instruction.Callee ?? "<null>") + "' without parameter metadata");
    }

    private static int InferParameterCountFromSubjectId(string subjectId)
    {
        // Format: "Namespace.Type::MethodName:ReturnType(Param1,Param2,...)"
        // Find the final top-level pair of parentheses containing parameter types.
        // Scan backward from end to find the outermost '(' that is at depth 0.
        int depth = 0;
        int lastOpenParen = -1;
        for (int i = subjectId.Length - 1; i >= 0; i--)
        {
            char c = subjectId[i];
            if (c is '>' or ']' or ')')
                depth++;
            else if (c is '<' or '[' or '(')
            {
                depth--;
                if (c == '(' && depth == 0)
                {
                    lastOpenParen = i;
                    break;
                }
            }
        }
        if (lastOpenParen < 0 || lastOpenParen >= subjectId.Length - 1)
            return 0;

        // Extract content between the parentheses
        string paramsPart = subjectId.Substring(lastOpenParen + 1);
        // Remove trailing ')'
        if (paramsPart.EndsWith(")"))
            paramsPart = paramsPart.Substring(0, paramsPart.Length - 1);

        if (string.IsNullOrEmpty(paramsPart))
            return 0;

        // Count top-level comma-separated parameters
        depth = 0;
        int count = 1;
        foreach (char ch in paramsPart)
        {
            if (ch is '<' or '[' or '(')
                depth++;
            else if (ch is '>' or ']' or ')')
                depth--;
            else if (ch == ',' && depth == 0)
                count++;
        }
        return count;
    }

    private static string? InferReturnTypeFromSubjectId(string subjectId)
    {
        // Format: "...Type::MethodName:ReturnType(Param1,Param2,...)"
        // Find the last '(' (start of parameter list), then scan backward to find
        // the ':' that separates method name from return type.
        int depth = 0;
        int lastOpenParen = -1;
        for (int i = subjectId.Length - 1; i >= 0; i--)
        {
            char c = subjectId[i];
            if (c is '>' or ']' or ')')
                depth++;
            else if (c is '<' or '[' or '(')
            {
                depth--;
                if (c == '(' && depth == 0)
                {
                    lastOpenParen = i;
                    break;
                }
            }
        }
        if (lastOpenParen < 0)
            return null;

        // Scan backward from lastOpenParen to find the last ':' before it
        int returnTypeEnd = lastOpenParen;
        int colonPos = -1;
        for (int i = returnTypeEnd - 1; i >= 0; i--)
        {
            if (subjectId[i] == ':')
            {
                colonPos = i;
                break;
            }
        }
        if (colonPos < 0)
            return null;

        string returnType = subjectId.Substring(colonPos + 1, returnTypeEnd - colonPos - 1);
        return string.IsNullOrEmpty(returnType) ? null : returnType;
    }

    private static AotCoreIrReferenceArtifact GetRequiredTargetReference(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.TargetReference is not null)
        {
            return instruction.TargetReference;
        }
        throw new NotSupportedException("native-aot lowering does not support opcode '" + instruction.Op + "' without target reference metadata");
    }

    private static string GetRequiredTypeHandleLiteral(AotCoreIrInstructionArtifact instruction)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        return requiredTargetReference.Kind switch
        {
            AotCoreIrReferenceKind.Type => GetTypeHandleLiteral(requiredTargetReference.SubjectId),
            AotCoreIrReferenceKind.Field => GetFieldHandleLiteral(requiredTargetReference.SubjectId),
            AotCoreIrReferenceKind.Method => GetMethodHandleLiteral(requiredTargetReference.SubjectId),
            _ => throw new NotSupportedException($"native-aot lowering does not support ldtoken target kind '{requiredTargetReference.Kind}'."),
        };
    }

    private static void EmitMethodReturn(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi)
    {
        string[] returnLines;
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Void:
                returnLines =
                [
                    "    return;"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Int32:
                returnLines =
                [
                    "    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Int8:
                returnLines =
                [
                    "    return static_cast<CHAOS_IL2CPP_INT8>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.UInt8:
                returnLines =
                [
                    "    return static_cast<CHAOS_IL2CPP_UINT8>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Int16:
                returnLines =
                [
                    "    return static_cast<CHAOS_IL2CPP_INT16>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.UInt16:
                returnLines =
                [
                    "    return static_cast<CHAOS_IL2CPP_UINT16>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Float32:
                returnLines =
                [
                    "    return ChaosLoadFloat32(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Float64:
                returnLines =
                [
                    "    return ChaosLoadFloat64(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Int64:
                returnLines =
                [
                    "    return ChaosLoadInt64(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.UInt64:
                returnLines =
                [
                    "    return chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            case AotCoreIrAbiCarrierKind.NativeInt:
            case AotCoreIrAbiCarrierKind.ByRef:
            case AotCoreIrAbiCarrierKind.MultiReturn:
            case AotCoreIrAbiCarrierKind.ByRefToValueType:
                returnLines =
                [
                    "    return chaos_eval_stack[--chaos_stack_top];"
                ];
                break;
            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                returnLines =
                [
                    $"    return *chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(returnAbi)}>(chaos_eval_stack[--chaos_stack_top]);"
                ];
                break;
            default:
                throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{returnAbi.CarrierKindCode}'.");
        }

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetMethodReturnTemplate(),
                new ScriptObject
                {
                    ["lines"] = returnLines,
                }).TrimEnd());
    }

    private void EmitAbiReturnPush(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi, string resultExpression, string indentation)
    {
        string[] pushLines;
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Int32:
            case AotCoreIrAbiCarrierKind.NativeInt:
            case AotCoreIrAbiCarrierKind.Int8:
            case AotCoreIrAbiCarrierKind.UInt8:
            case AotCoreIrAbiCarrierKind.Int16:
            case AotCoreIrAbiCarrierKind.UInt16:
            case AotCoreIrAbiCarrierKind.ByRef:
            case AotCoreIrAbiCarrierKind.MultiReturn:
            case AotCoreIrAbiCarrierKind.ByRefToValueType:
                pushLines =
                [
                    $"{indentation}{AllocateEvalStackTargetExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>({resultExpression});"
                ];
                break;
            case AotCoreIrAbiCarrierKind.Float32:
                pushLines =
                [
                    $"{indentation}{AllocateEvalStackTargetExpression()} = ChaosStoreFloat32({resultExpression});"
                ];
                PushSlotType(SlotType.Float32);
                break;
            case AotCoreIrAbiCarrierKind.Float64:
                pushLines =
                [
                    $"{indentation}{AllocateEvalStackTargetExpression()} = ChaosStoreFloat64({resultExpression});"
                ];
                PushSlotType(SlotType.Float64);
                break;
            case AotCoreIrAbiCarrierKind.Int64:
                pushLines =
                [
                    $"{indentation}{AllocateEvalStackTargetExpression()} = ChaosStoreInt64({resultExpression});"
                ];
                break;
            case AotCoreIrAbiCarrierKind.UInt64:
                pushLines =
                [
                    $"{indentation}{AllocateEvalStackTargetExpression()} = chaos_store_uint64({resultExpression});"
                ];
                break;
            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                pushLines =
                [
                    $"{indentation}auto* chaos_result_storage = new {GetRequiredAbiValueTypeSymbol(returnAbi)}{{}};",
                $"{indentation}*chaos_result_storage = {resultExpression};",
                $"{indentation}{AllocateEvalStackTargetExpression()} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_result_storage);",
            ];
                break;
            default:
                throw new NotSupportedException($"native-aot lowering does not support pushing ABI return carrier '{returnAbi.CarrierKindCode}'.");
        }

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetAbiReturnPushTemplate(),
                new ScriptObject
                {
                    ["lines"] = pushLines,
                }).TrimEnd());
    }

    /// <summary>
    /// Returns only the C++ parameter types without argument names.
    /// Used when constructing function-pointer type signatures in
    /// reinterpret_cast expressions where named parameters would shadow
    /// local variables.
    /// </summary>

    // ── ArgBuffer helpers for dispatch-table-aware call sites (Hotpatch) ──

    /// <summary>
    /// Returns the byte size needed in an ArgBuffer for a single ABI slot.
    /// </summary>

    /// <summary>
    /// Calculate total ArgBuffer size needed for a list of parameter ABIs.
    /// </summary>

    /// <summary>
    /// Returns the ArgBuffer WriteXxx call expression for a parameter ABI slot.
    /// The arg value is expected to be in a CHAOS_IL2CPP_INTPTR variable named argName.
    /// Example: "_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0))"
    /// </summary>

}
