using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{



    private static bool TryMapPointerParameterToCppType(string managedType, out string cppType)
    {
        cppType = string.Empty;
        if (string.IsNullOrWhiteSpace(managedType) || !managedType.EndsWith("*", StringComparison.Ordinal))
        {
            return false;
        }

        var elementManagedType = managedType[..^1];
        if (!TryMapManagedScalarCppType(elementManagedType, out var elementCppType))
        {
            return false;
        }

        cppType = $"{elementCppType}*";
        return true;
    }





    private static string GetCarrierCppTypeForFixedVectorWidth(int fixedVectorWidthBytes)
    {
        return fixedVectorWidthBytes switch
        {
            8 => "RuntimeIntrinsicVector64Carrier",
            16 => "RuntimeIntrinsicVector128Carrier",
            32 => "RuntimeIntrinsicVector256Carrier",
            64 => "RuntimeIntrinsicVector512Carrier",
            _ => throw new InvalidOperationException($"unsupported fixed vector width '{fixedVectorWidthBytes}'"),
        };
    }





    private static int GetFixedVectorWidthBytesFromCarrier(string carrierCppType)
    {
        return carrierCppType switch
        {
            "RuntimeIntrinsicVector64Carrier" => 8,
            "RuntimeIntrinsicVector128Carrier" => 16,
            "RuntimeIntrinsicVector256Carrier" => 32,
            "RuntimeIntrinsicVector512Carrier" => 64,
            _ => throw new InvalidOperationException($"unsupported carrier type '{carrierCppType}'"),
        };
    }





    private static IReadOnlyList<string> SplitTopLevelParameters(string parameterSignature)
    {
        if (string.IsNullOrWhiteSpace(parameterSignature))
        {
            return [];
        }

        var parameters = new List<string>();
        var current = new System.Text.StringBuilder();
        var genericDepth = 0;
        foreach (var character in parameterSignature)
        {
            switch (character)
            {
                case '<':
                    genericDepth++;
                    current.Append(character);
                    break;
                case '>':
                    genericDepth--;
                    current.Append(character);
                    break;
                case ',' when genericDepth == 0:
                    parameters.Add(current.ToString());
                    current.Clear();
                    break;
                default:
                    current.Append(character);
                    break;
            }
        }

        if (current.Length > 0)
        {
            parameters.Add(current.ToString());
        }

        return parameters;
    }





    private static bool TryResolveShiftCountShape(
        string managedType,
        out string cppType,
        out string valueExpression)
    {
        cppType = "CHAOS_IL2CPP_INT32";
        valueExpression = "request->arg1";

        if (string.Equals(managedType, "System.Int32", StringComparison.Ordinal))
        {
            return true;
        }

        if (string.Equals(managedType, "System.Byte", StringComparison.Ordinal))
        {
            cppType = "CHAOS_IL2CPP_UINT8";
            valueExpression = "static_cast<CHAOS_IL2CPP_INT32>(request->arg1)";
            return true;
        }

        if (TryResolveCarrierFromManagedVectorType(
                managedType,
                out var scalarManagedType,
                out var carrierCppType,
                out _)
            && TryMapManagedScalarCppType(scalarManagedType, out var scalarCppType))
        {
            cppType = carrierCppType;
            valueExpression =
                $"{GetHelperNamespace()}::VectorFixedExtractShiftCount<{scalarCppType}, {carrierCppType}>(request->arg1)";
            return true;
        }

        return false;
    }





    private static bool TryResolveNumericsVectorScalarSelector(
        string subjectIdPrefix,
        string subjectId,
        out string? scalarTypeSubjectId,
        out RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind,
        out int? scalarGenericArgumentIndex)
    {
        scalarTypeSubjectId = null;
        scalarResolutionKind = null;
        scalarGenericArgumentIndex = null;

        if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
        {
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ClassGenericArgument;
            scalarGenericArgumentIndex = 0;
            return true;
        }

        if (!TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out var scalarManagedType, out _))
        {
            return false;
        }

        if (TryMapManagedScalarSubjectId(scalarManagedType, out var concreteSubjectId))
        {
            scalarTypeSubjectId = concreteSubjectId;
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ConcreteTypeToken;
            return true;
        }

        if (TryParseGenericArgumentSelector(scalarManagedType, out var parsedResolutionKind, out var parsedIndex))
        {
            scalarResolutionKind = parsedResolutionKind;
            scalarGenericArgumentIndex = parsedIndex;
            return true;
        }

        return false;
    }





    private static bool TryCreateFixedConstantPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryExtractIntrinsicVectorMethodSuffix(descriptor.SubjectIdPrefix, subjectId, out var methodSuffix))
        {
            return false;
        }

        return methodSuffix switch
        {
            var current when current.StartsWith("get_Zero:", StringComparison.Ordinal) =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{descriptor.CarrierCppType}{{}}",
                    "vector-fixed-constant",
                    out plan),
            var current when current.StartsWith("get_AllBitsSet:", StringComparison.Ordinal) =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedOnesComplement<{descriptor.CarrierCppType}>({descriptor.CarrierCppType}{{}})",
                    "vector-fixed-constant",
                    out plan),
            _ => false,
        };
    }

}
