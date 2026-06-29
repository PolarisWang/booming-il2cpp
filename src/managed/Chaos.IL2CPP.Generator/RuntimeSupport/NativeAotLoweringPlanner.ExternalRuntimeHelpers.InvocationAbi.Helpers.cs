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

    private static bool CanEmitMethodBody(AotCoreIrMethodArtifact method)
    {
        return method.Instructions.Count > 0 || method.IsPInvoke;
    }



    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> GetMethodAbiParameterSlots(AotCoreIrMethodArtifact method)
    {
        ArgumentNullException.ThrowIfNull(method, "method");
        if (method.IsStatic)
        {
            return method.ParameterAbis;
        }
        List<AotCoreIrAbiSlotArtifact> list = new List<AotCoreIrAbiSlotArtifact>(checked(method.ParameterAbis.Count + 1));
        list.Add(CreateNativeIntAbiSlot(method.Identity.DeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType));
        list.AddRange(method.ParameterAbis);
        return list;
    }



    private static AotCoreIrAbiSlotArtifact GetRequiredMethodAbiParameterSlot(AotCoreIrMethodArtifact method, int argumentIndex)
    {
        IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
        if (argumentIndex < 0 || argumentIndex >= methodAbiParameterSlots.Count)
        {
            throw new InvalidOperationException($"native-aot lowering could not resolve argument slot {argumentIndex} for '{method.SubjectId}'.");
        }
        return methodAbiParameterSlots[argumentIndex];
    }



    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> CreateLegacyAbiParameterSlots(int parameterCount)
    {
        if (parameterCount < 0)
        {
            throw new NotSupportedException("native-aot lowering requires a non-negative parameter count.");
        }
        if (parameterCount == 0)
        {
            return Array.Empty<AotCoreIrAbiSlotArtifact>();
        }
        return (from _ in Enumerable.Range(0, parameterCount)
                select CreateNativeIntAbiSlot()).ToArray();
    }



    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> CreateCallSiteParameterAbis(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.CallSiteSignature?.ParameterTypes == null)
        {
            throw new NotSupportedException("native-aot lowering requires call-site signature metadata for '" + instruction.Op + "'.");
        }
        return instruction.CallSiteSignature.ParameterTypes.Select(CreateLegacyAbiSlot).ToArray();
    }



    private static AotCoreIrAbiSlotArtifact CreateCallSiteReturnAbi(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.CallSiteSignature is null)
        {
            throw new NotSupportedException("native-aot lowering requires call-site signature metadata for '" + instruction.Op + "'.");
        }
        return CreateLegacyReturnAbiSlot(instruction.CallSiteSignature.ReturnType);
    }



    private static AotCoreIrAbiSlotArtifact CreateLegacyReturnAbiSlot(string? returnType)
    {
        switch (returnType)
        {
            case "System.Void":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
                    TypeShape = (AotCoreIrTypeShapeKind)0
                };
            case "System.Int32":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.SByte":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Byte":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Int16":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.UInt16":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Single":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Double":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Int64":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.UInt64":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            default:
                if (!string.IsNullOrEmpty(returnType))
                {
                    return CreateLegacyAbiSlot(returnType);
                }
                throw new NotSupportedException("native-aot lowering does not support unresolved legacy return type '" + (returnType ?? "<null>") + "'.");
        }
    }



    private static AotCoreIrAbiSlotArtifact CreateLegacyAbiSlot(string? typeName)
    {
        switch (typeName)
        {
            case "System.Boolean":
                return CreateNativeIntAbiSlot();
            case "System.Byte":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.SByte":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Int16":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.UInt16":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Int32":
                return CreateInt32AbiSlot();
            case "System.UInt32":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Single":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Double":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.Int64":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            case "System.UInt64":
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType
                };
            default:
                if (!string.IsNullOrEmpty(typeName))
                {
                    return CreateNativeIntAbiSlot();
                }
                throw new NotSupportedException("native-aot lowering does not support unresolved legacy ABI type '" + (typeName ?? "<null>") + "'.");
        }
    }



    private static AotCoreIrAbiSlotArtifact CreateNativeIntAbiSlot(string? typeSubjectId = null, AotCoreIrTypeShapeKind typeShape = (AotCoreIrTypeShapeKind)0)
    {
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeSubjectId = typeSubjectId,
            TypeShape = typeShape
        };
    }



    private static AotCoreIrAbiSlotArtifact CreateInt32AbiSlot(string? typeSubjectId = null, AotCoreIrTypeShapeKind typeShape = (AotCoreIrTypeShapeKind)0)
    {
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
            TypeSubjectId = typeSubjectId,
            TypeShape = typeShape
        };
    }



    private static AotCoreIrAbiSlotArtifact CreateInt64AbiSlot(string? typeSubjectId = null, AotCoreIrTypeShapeKind typeShape = (AotCoreIrTypeShapeKind)0)
    {
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
            TypeSubjectId = typeSubjectId,
            TypeShape = typeShape
        };
    }



    private static string MapAbiSlotReturnType(AotCoreIrAbiSlotArtifact abiSlot)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Void => "void",
            AotCoreIrAbiCarrierKind.Int32 => "CHAOS_IL2CPP_INT32",
            AotCoreIrAbiCarrierKind.Int8 => "CHAOS_IL2CPP_INT8",
            AotCoreIrAbiCarrierKind.UInt8 => "CHAOS_IL2CPP_UINT8",
            AotCoreIrAbiCarrierKind.Int16 => "CHAOS_IL2CPP_INT16",
            AotCoreIrAbiCarrierKind.UInt16 => "CHAOS_IL2CPP_UINT16",
            AotCoreIrAbiCarrierKind.Float32 => "float",
            AotCoreIrAbiCarrierKind.Float64 => "double",
            AotCoreIrAbiCarrierKind.Int64 => "CHAOS_IL2CPP_INT64",
            AotCoreIrAbiCarrierKind.UInt64 => "CHAOS_IL2CPP_UINT64",
            AotCoreIrAbiCarrierKind.NativeInt => MapNativeIntSlot(abiSlot),
            AotCoreIrAbiCarrierKind.ByRef => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.MultiReturn => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.ByRefToValueType => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot),
            _ => throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{abiSlot.CarrierKindCode}'."),
        };
    }

    /// <summary>
    /// Map a NativeInt ABI slot to a C++ type name.  When the slot has a
    /// non-null TypeSubjectId from an external value type (e.g. System.Data.
    /// CommandBehavior), emit chaos_valuetype_X instead of CHAOS_IL2CPP_INTPTR.
    /// This ensures dispatch table typedefs in chaos_generated_module.h match
    /// the extern "C" declarations in page files.
    /// </summary>
    private static string MapNativeIntSlot(AotCoreIrAbiSlotArtifact abiSlot)
    {
        if (!string.IsNullOrEmpty(abiSlot.TypeSubjectId) &&
            !abiSlot.TypeSubjectId.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal))
            return GetNativeValueTypeSymbol(abiSlot.TypeSubjectId);
        return "CHAOS_IL2CPP_INTPTR";
    }



    private static string FormatAbiSlotParameterSignature(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
    {
        ArgumentNullException.ThrowIfNull(abiSlots, "abiSlots");
        if (abiSlots.Count == 0)
        {
            return "void";
        }
        return string.Join(", ", abiSlots.Select((AotCoreIrAbiSlotArtifact slot, int index) => $"{MapAbiSlotParameterType(slot)} chaos_fn_arg_{index}"));
    }



    /// <summary>
    /// Returns only the C++ parameter types without argument names.
    /// Used when constructing function-pointer type signatures in
    /// reinterpret_cast expressions where named parameters would shadow
    /// local variables.
    /// </summary>
    private static string FormatAbiSlotParameterTypes(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
    {
        ArgumentNullException.ThrowIfNull(abiSlots, "abiSlots");
        if (abiSlots.Count == 0)
        {
            return "void";
        }
        return string.Join(", ", abiSlots.Select(slot => MapAbiSlotParameterType(slot)));
    }



    private static void EmitAbiArgumentInitialization(StringBuilder builder, IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
    {
        var lines = new List<string>();
        for (int i = 0; i < abiSlots.Count; i++)
        {
            AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = abiSlots[i];
            switch (aotCoreIrAbiSlotArtifact.CarrierKindCode)
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
                    lines.Add($"    chaos_args[{i}] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_{i});");
                    break;
                case AotCoreIrAbiCarrierKind.Float32:
                    lines.Add($"    chaos_args[{i}] = ChaosStoreFloat32(chaos_fn_arg_{i});");
                    break;
                case AotCoreIrAbiCarrierKind.Float64:
                    lines.Add($"    chaos_args[{i}] = ChaosStoreFloat64(chaos_fn_arg_{i});");
                    break;
                case AotCoreIrAbiCarrierKind.Int64:
                    lines.Add($"    chaos_args[{i}] = ChaosStoreInt64(chaos_fn_arg_{i});");
                    break;
                case AotCoreIrAbiCarrierKind.UInt64:
                    lines.Add($"    chaos_args[{i}] = chaos_store_uint64(chaos_fn_arg_{i});");
                    break;
                case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                    lines.Add($"    auto chaos_abi_param_{i} = chaos_fn_arg_{i};");
                    lines.Add($"    chaos_args[{i}] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_abi_param_{i});");
                    break;
                default:
                    throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{aotCoreIrAbiSlotArtifact.CarrierKindCode}'.");
            }
        }

        builder.AppendLine(
            ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetAbiArgumentInitializationTemplate(),
                new ScriptObject
                {
                    ["lines"] = lines.ToArray(),
                }).TrimEnd());
    }



    private static string FormatAbiInvocationArgumentList(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots, string? firstArgumentOverride = null)
    {
        if (abiSlots.Count == 0)
        {
            return string.Empty;
        }
        return string.Join(", ", abiSlots.Select(delegate (AotCoreIrAbiSlotArtifact slot, int index)
        {
            string sourceName = ((index == 0 && !string.IsNullOrWhiteSpace(firstArgumentOverride)) ? firstArgumentOverride : $"chaos_arg_{index}");
            return FormatAbiArgumentExpression(slot, sourceName);
        }));
    }



    private static string FormatAbiArgumentExpression(AotCoreIrAbiSlotArtifact abiSlot, string sourceName)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Int32 => "static_cast<CHAOS_IL2CPP_INT32>(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.Int8 => "static_cast<CHAOS_IL2CPP_INT8>(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.UInt8 => "static_cast<CHAOS_IL2CPP_UINT8>(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.Int16 => "static_cast<CHAOS_IL2CPP_INT16>(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.UInt16 => "static_cast<CHAOS_IL2CPP_UINT16>(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.Float32 => "ChaosLoadFloat32(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.Float64 => "ChaosLoadFloat64(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.Int64 => "ChaosLoadInt64(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.UInt64 => "chaos_load_uint64(" + sourceName + ")",
            AotCoreIrAbiCarrierKind.NativeInt => sourceName,
            AotCoreIrAbiCarrierKind.ByRef => sourceName,
            AotCoreIrAbiCarrierKind.MultiReturn => sourceName,
            AotCoreIrAbiCarrierKind.ByRefToValueType => sourceName,
            AotCoreIrAbiCarrierKind.ValueTypeByValue => $"*chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(abiSlot)}>({sourceName})",
            _ => throw new NotSupportedException($"native-aot lowering does not support ABI argument carrier '{abiSlot.CarrierKindCode}'."),
        };
    }



    private static string FormatInboundAbiArgumentExpression(AotCoreIrAbiSlotArtifact abiSlot, string sourceName)
    {
        if (abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt
            && abiSlot.TypeShape == AotCoreIrTypeShapeKind.ReferenceType
            && !IsStringParameterSlot(abiSlot))
        {
            return "chaos_normalize_native_int_argument(" + sourceName + ")";
        }

        if (abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRef
            || abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRefToValueType
            || abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.MultiReturn)
        {
            return "chaos_normalize_native_int_argument(" + sourceName + ")";
        }
        return sourceName;
    }



    private static bool IsStringParameterSlot(AotCoreIrAbiSlotArtifact abiSlot)
    {
        return abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt &&
            string.Equals(abiSlot.TypeSubjectId, "System.Private.CoreLib/System.String", StringComparison.Ordinal);
    }



    private static string MapAbiSlotParameterType(AotCoreIrAbiSlotArtifact abiSlot)
    {
        return abiSlot.CarrierKindCode switch
        {
            AotCoreIrAbiCarrierKind.Int32 => "CHAOS_IL2CPP_INT32",
            AotCoreIrAbiCarrierKind.Int8 => "CHAOS_IL2CPP_INT8",
            AotCoreIrAbiCarrierKind.UInt8 => "CHAOS_IL2CPP_UINT8",
            AotCoreIrAbiCarrierKind.Int16 => "CHAOS_IL2CPP_INT16",
            AotCoreIrAbiCarrierKind.UInt16 => "CHAOS_IL2CPP_UINT16",
            AotCoreIrAbiCarrierKind.Float32 => "float",
            AotCoreIrAbiCarrierKind.Float64 => "double",
            AotCoreIrAbiCarrierKind.Int64 => "CHAOS_IL2CPP_INT64",
            AotCoreIrAbiCarrierKind.UInt64 => "CHAOS_IL2CPP_UINT64",
            AotCoreIrAbiCarrierKind.NativeInt => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.ByRef => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.MultiReturn => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.ByRefToValueType => "CHAOS_IL2CPP_INTPTR",
            AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot),
            _ => throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{abiSlot.CarrierKindCode}'."),
        };
    }



    private static string GetRequiredAbiValueTypeSymbol(AotCoreIrAbiSlotArtifact abiSlot)
    {
        if (abiSlot.CarrierKindCode != AotCoreIrAbiCarrierKind.ValueTypeByValue || string.IsNullOrEmpty(abiSlot.TypeSubjectId))
        {
            throw new NotSupportedException($"native-aot lowering requires a value-type ABI slot with subject metadata, got '{abiSlot.CarrierKindCode}'.");
        }
        return GetNativeValueTypeSymbol(abiSlot.TypeSubjectId);
    }



    private static string FormatInt32Literal(int value)
    {
        if (value != int.MinValue)
        {
            return value.ToString(CultureInfo.InvariantCulture);
        }
        return "CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32)";
    }



    private static string FormatInt64Literal(long value)
    {
        if (value != long.MinValue)
        {
            return value.ToString(CultureInfo.InvariantCulture) + "LL";
        }
        return "CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT64)";
    }



    private static string FormatFloat32Literal(float value)
    {
        var s = value.ToString("R", CultureInfo.InvariantCulture);
        // C++ requires a decimal point for float literals with f suffix
        // (e.g. 42f is invalid, must be 42.0f)
        if (!s.Contains('.') && !s.Contains('e') && !s.Contains('E'))
            s += ".0";
        return s + "f";
    }



    private static string FormatFloat64Literal(double value)
    {
        return value.ToString("R", CultureInfo.InvariantCulture);
    }



    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrEmpty(value))
        {
            throw new InvalidOperationException("native-aot lowering plan requires non-empty field '" + fieldName + "'");
        }
    }



    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("required native-aot lowering plan is missing: " + path, path);
        }
        return JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions) ?? throw new InvalidOperationException("failed to deserialize native-aot lowering plan: " + path);
    }



    private static string? TryGetInstantiationStubSymbol(InstantiationStubId? instantiationStubId)
    {
        if (instantiationStubId is null)
        {
            return null;
        }

        return ManagedNaming.CreateInstantiationStubSymbol(instantiationStubId);
    }



    // ── ArgBuffer helpers for dispatch-table-aware call sites (Hotpatch) ──

    /// <summary>
    /// Returns the byte size needed in an ArgBuffer for a single ABI slot.
    /// </summary>
    private static int GetAbiSlotArgBufferSize(AotCoreIrAbiCarrierKind kind)
    {
        switch (kind)
        {
            case AotCoreIrAbiCarrierKind.Int32:
            case AotCoreIrAbiCarrierKind.Int8:
            case AotCoreIrAbiCarrierKind.UInt8:
            case AotCoreIrAbiCarrierKind.Int16:
            case AotCoreIrAbiCarrierKind.UInt16:
            case AotCoreIrAbiCarrierKind.Float32:
                return 4;
            case AotCoreIrAbiCarrierKind.Float64:
            case AotCoreIrAbiCarrierKind.Int64:
            case AotCoreIrAbiCarrierKind.UInt64:
            case AotCoreIrAbiCarrierKind.NativeInt:
            case AotCoreIrAbiCarrierKind.ByRef:
            case AotCoreIrAbiCarrierKind.MultiReturn:
            case AotCoreIrAbiCarrierKind.ByRefToValueType:
            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                return 8;
            default:
                return 8;
        }
    }



    /// <summary>
    /// Calculate total ArgBuffer size needed for a list of parameter ABIs.
    /// </summary>
    private static int CalculateArgBufferSize(IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis)
    {
        int size = 0;
        for (int i = 0; i < parameterAbis.Count; i++)
            size += GetAbiSlotArgBufferSize(parameterAbis[i].CarrierKindCode);
        return size;
    }



    /// <summary>
    /// Returns the ArgBuffer WriteXxx call expression for a parameter ABI slot.
    /// The arg value is expected to be in a CHAOS_IL2CPP_INTPTR variable named argName.
    /// Example: "_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0))"
    /// </summary>
    private static string GetArgBufferWriteCall(AotCoreIrAbiCarrierKind kind, string argName)
    {
        switch (kind)
        {
            case AotCoreIrAbiCarrierKind.Int32:
                return $"WriteI32(static_cast<CHAOS_IL2CPP_INT32>({argName}))";
            case AotCoreIrAbiCarrierKind.Int8:
            case AotCoreIrAbiCarrierKind.UInt8:
            case AotCoreIrAbiCarrierKind.Int16:
            case AotCoreIrAbiCarrierKind.UInt16:
                return $"WriteI32({argName})";
            case AotCoreIrAbiCarrierKind.Float32:
                return $"WriteF32(ChaosLoadFloat32({argName}))";
            case AotCoreIrAbiCarrierKind.Float64:
                return $"WriteF64(ChaosLoadFloat64({argName}))";
            case AotCoreIrAbiCarrierKind.Int64:
                return $"WriteI64(ChaosLoadInt64({argName}))";
            case AotCoreIrAbiCarrierKind.UInt64:
                return $"WriteI64(chaos_load_uint64({argName}))";
            case AotCoreIrAbiCarrierKind.NativeInt:
            case AotCoreIrAbiCarrierKind.ByRef:
            case AotCoreIrAbiCarrierKind.MultiReturn:
            case AotCoreIrAbiCarrierKind.ByRefToValueType:
            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                return $"WritePtr(reinterpret_cast<void*>({argName}))";
            default:
                return $"WritePtr(reinterpret_cast<void*>({argName}))";
        }
    }

}
