using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{
    private static bool TryCreatePlanCore(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string semanticId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        RuntimeSkeletonVectorKernelCarrierSchema carrierSchema,
        string helperCallExpression,
        out RuntimeSkeletonVectorKernelEmissionPlan plan) =>
        TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            carrierSchema,
            helperCallExpression,
            null,
            null,
            null,
            null,
            null,
            out plan);

    private static string GetManagedVectorTypeName(RuntimeSkeletonVectorKernelSemanticDescriptor descriptor)
    {
        return descriptor.SubjectIdPrefix switch
        {
            "/System.Numerics.Vector2::" => "System.Numerics.Vector2",
            "/System.Numerics.Vector3::" => "System.Numerics.Vector3",
            "/System.Numerics.Vector4::" => "System.Numerics.Vector4",
            _ => throw new InvalidOperationException(
                $"unsupported managed vector type prefix '{descriptor.SubjectIdPrefix}'"),
        };
    }

    private static bool TryExtractClosedIntrinsicScalarManagedType(
        string subjectIdPrefix,
        string subjectId,
        out string scalarManagedType,
        out string methodSuffix)
    {
        scalarManagedType = string.Empty;
        methodSuffix = string.Empty;

        var prefixIndex = subjectId.IndexOf(subjectIdPrefix, StringComparison.Ordinal);
        if (prefixIndex < 0)
        {
            return false;
        }

        var scalarStartIndex = prefixIndex + subjectIdPrefix.Length;
        var scalarEndIndex = subjectId.IndexOf(">::", scalarStartIndex, StringComparison.Ordinal);
        if (scalarEndIndex <= scalarStartIndex)
        {
            return false;
        }

        scalarManagedType = subjectId[scalarStartIndex..scalarEndIndex];
        methodSuffix = subjectId[(scalarEndIndex + 3)..];
        return true;
    }

    private static bool TryMapManagedScalarCppType(string managedType, out string cppType)
    {
        cppType = managedType switch
        {
            "System.Byte" => "CHAOS_IL2CPP_UINT8",
            "System.SByte" => "CHAOS_IL2CPP_INT8",
            "System.Int16" => "CHAOS_IL2CPP_INT16",
            "System.UInt16" => "CHAOS_IL2CPP_UINT16",
            "System.Int32" => "CHAOS_IL2CPP_INT32",
            "System.UInt32" => "CHAOS_IL2CPP_UINT32",
            "System.Int64" => "CHAOS_IL2CPP_INT64",
            "System.UInt64" => "CHAOS_IL2CPP_UINT64",
            "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
            "System.UIntPtr" => "CHAOS_IL2CPP_UINTPTR",
            "System.Single" => "float",
            "System.Double" => "double",
            _ => string.Empty,
        };
        return !string.IsNullOrWhiteSpace(cppType);
    }

    private static bool TryMapManagedScalarSubjectId(string managedType, out string subjectId)
    {
        subjectId = managedType switch
        {
            "System.Byte" => "System.Private.CoreLib/System.Byte",
            "System.SByte" => "System.Private.CoreLib/System.SByte",
            "System.Int16" => "System.Private.CoreLib/System.Int16",
            "System.UInt16" => "System.Private.CoreLib/System.UInt16",
            "System.Int32" => "System.Private.CoreLib/System.Int32",
            "System.UInt32" => "System.Private.CoreLib/System.UInt32",
            "System.Int64" => "System.Private.CoreLib/System.Int64",
            "System.UInt64" => "System.Private.CoreLib/System.UInt64",
            "System.IntPtr" => "System.Private.CoreLib/System.IntPtr",
            "System.UIntPtr" => "System.Private.CoreLib/System.UIntPtr",
            "System.Single" => "System.Private.CoreLib/System.Single",
            "System.Double" => "System.Private.CoreLib/System.Double",
            _ => string.Empty,
        };
        return !string.IsNullOrWhiteSpace(subjectId);
    }

    private static bool TryResolveShiftTrait(
        string managedType,
        out RuntimeSkeletonVectorShiftTraitKind shiftTraitKind)
    {
        if (TryParseGenericArgumentSelector(managedType, out _, out _))
        {
            shiftTraitKind = RuntimeSkeletonVectorShiftTraitKind.Deferred;
            return true;
        }

        shiftTraitKind = managedType switch
        {
            "System.SByte" or "System.Int16" or "System.Int32" or "System.Int64" => RuntimeSkeletonVectorShiftTraitKind.Signed,
            "System.Byte" or "System.UInt16" or "System.UInt32" or "System.UInt64" => RuntimeSkeletonVectorShiftTraitKind.Unsigned,
            "System.IntPtr" => RuntimeSkeletonVectorShiftTraitKind.NativeSigned,
            "System.UIntPtr" => RuntimeSkeletonVectorShiftTraitKind.NativeUnsigned,
            "System.Single" or "System.Double" => RuntimeSkeletonVectorShiftTraitKind.Floating,
            _ => RuntimeSkeletonVectorShiftTraitKind.None,
        };
        return shiftTraitKind != RuntimeSkeletonVectorShiftTraitKind.None;
    }

    private static bool TryResolveShiftScalarSelector(
        string subjectIdPrefix,
        string subjectId,
        string scalarManagedType,
        out string? scalarTypeSubjectId,
        out RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind,
        out int? scalarGenericArgumentIndex)
    {
        scalarTypeSubjectId = null;
        scalarResolutionKind = null;
        scalarGenericArgumentIndex = null;

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

        if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
        {
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ClassGenericArgument;
            scalarGenericArgumentIndex = 0;
            return true;
        }

        return false;
    }

    private static bool TryResolveCapabilityScalarSelector(
        string subjectIdPrefix,
        string subjectId,
        out string methodSuffix,
        out string? scalarTypeSubjectId,
        out RuntimeSkeletonVectorKernelScalarResolutionKind scalarResolutionKind,
        out int? scalarGenericArgumentIndex)
    {
        methodSuffix = string.Empty;
        scalarTypeSubjectId = null;
        scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ConcreteTypeToken;
        scalarGenericArgumentIndex = null;

        if (subjectIdPrefix.StartsWith("/System.Numerics.Vector", StringComparison.Ordinal))
        {
            if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
            {
                if (!TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix))
                {
                    return false;
                }

                scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ClassGenericArgument;
                scalarGenericArgumentIndex = 0;
                return true;
            }

            if (!TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out var numericsScalarManagedType, out methodSuffix))
            {
                return false;
            }

            if (TryMapManagedScalarSubjectId(numericsScalarManagedType, out var numericsConcreteSubjectId))
            {
                scalarTypeSubjectId = numericsConcreteSubjectId;
                scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ConcreteTypeToken;
                return true;
            }

            if (TryParseGenericArgumentSelector(
                    numericsScalarManagedType,
                    out scalarResolutionKind,
                    out var numericsGenericArgumentIndex))
            {
                scalarGenericArgumentIndex = numericsGenericArgumentIndex;
                return true;
            }

            return false;
        }

        if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
        {
            if (!TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix))
            {
                return false;
            }

            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ClassGenericArgument;
            scalarGenericArgumentIndex = 0;
            return true;
        }

        if (TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix))
        {
            return true;
        }

        if (!TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out var scalarManagedType, out methodSuffix))
        {
            return false;
        }

        if (TryMapManagedScalarSubjectId(scalarManagedType, out var concreteSubjectId))
        {
            scalarTypeSubjectId = concreteSubjectId;
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ConcreteTypeToken;
            return true;
        }

        if (TryParseGenericArgumentSelector(
                scalarManagedType,
                out scalarResolutionKind,
                out var genericArgumentIndex))
        {
            scalarGenericArgumentIndex = genericArgumentIndex;
            return true;
        }

        return false;
    }

    private static bool TryExtractOpenIntrinsicMethodSuffix(
        string subjectIdPrefix,
        string subjectId,
        out string methodSuffix)
    {
        methodSuffix = string.Empty;
        var prefixIndex = subjectId.IndexOf(subjectIdPrefix, StringComparison.Ordinal);
        if (prefixIndex < 0)
        {
            return false;
        }

        var suffixStartIndex = prefixIndex + subjectIdPrefix.Length;
        if (suffixStartIndex >= subjectId.Length)
        {
            return false;
        }

        methodSuffix = subjectId[suffixStartIndex..];
        return !string.IsNullOrWhiteSpace(methodSuffix);
    }

    private static bool TryExtractIntrinsicVectorMethodSuffix(
        string subjectIdPrefix,
        string subjectId,
        out string methodSuffix)
    {
        if (subjectIdPrefix.StartsWith("/System.Numerics.Vector", StringComparison.Ordinal))
        {
            if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
            {
                return TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix);
            }

            if (TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out _, out methodSuffix))
            {
                return true;
            }

            methodSuffix = string.Empty;
            return false;
        }

        if (subjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
        {
            return TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix);
        }

        if (TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out _, out methodSuffix))
        {
            return true;
        }

        methodSuffix = string.Empty;
        return false;
    }

    private static bool TryExtractIntrinsicFactoryShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string returnManagedVectorType,
        out string returnScalarManagedType,
        out string parameterSignature)
    {
        methodName = string.Empty;
        returnManagedVectorType = string.Empty;
        returnScalarManagedType = string.Empty;
        parameterSignature = string.Empty;

        var prefixIndex = subjectId.IndexOf(subjectIdPrefix, StringComparison.Ordinal);
        if (prefixIndex < 0)
        {
            return false;
        }

        var suffix = subjectId[(prefixIndex + subjectIdPrefix.Length)..];
        var methodNameEnd = suffix.IndexOf(':');
        if (methodNameEnd <= 0)
        {
            return false;
        }

        methodName = suffix[..methodNameEnd];
        var returnAndParameters = suffix[(methodNameEnd + 1)..];
        return TryExtractIntrinsicFactoryReturnVectorType(
            subjectIdPrefix,
            returnAndParameters,
            out returnManagedVectorType,
            out returnScalarManagedType,
            out parameterSignature);
    }

    private static bool TryExtractIntrinsicFactoryReturnVectorType(
        string subjectIdPrefix,
        string returnAndParameters,
        out string returnManagedVectorType,
        out string returnScalarManagedType,
        out string parameterSignature)
    {
        returnManagedVectorType = string.Empty;
        returnScalarManagedType = string.Empty;
        parameterSignature = string.Empty;

        foreach (var vectorTypePrefix in new[]
                 {
                     "System.Numerics.Vector<",
                     "System.Runtime.Intrinsics.Vector64<",
                     "System.Runtime.Intrinsics.Vector128<",
                     "System.Runtime.Intrinsics.Vector256<",
                     "System.Runtime.Intrinsics.Vector512<",
                 })
        {
            if (!returnAndParameters.StartsWith(vectorTypePrefix, StringComparison.Ordinal))
            {
                continue;
            }

            var returnScalarStart = vectorTypePrefix.Length;
            var returnScalarEnd = returnAndParameters.IndexOf(">(", returnScalarStart, StringComparison.Ordinal);
            if (returnScalarEnd <= returnScalarStart)
            {
                return false;
            }

            var candidateReturnScalarManagedType = returnAndParameters[returnScalarStart..returnScalarEnd];
            var candidateReturnManagedVectorType = $"{vectorTypePrefix}{candidateReturnScalarManagedType}>";
            if (!TryIsReturnManagedVectorTypeCompatible(subjectIdPrefix, candidateReturnManagedVectorType))
            {
                continue;
            }

            var parameterStart = returnScalarEnd + 2;
            var parameterEnd = returnAndParameters.LastIndexOf(')');
            if (parameterEnd < parameterStart)
            {
                return false;
            }

            returnManagedVectorType = candidateReturnManagedVectorType;
            returnScalarManagedType = candidateReturnScalarManagedType;
            parameterSignature = returnAndParameters[parameterStart..parameterEnd];
            return true;
        }

        return false;
    }

    private static bool TryIsReturnManagedVectorTypeCompatible(string subjectIdPrefix, string returnManagedVectorType)
    {
        if (subjectIdPrefix.StartsWith("/System.Numerics.Vector", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Numerics.Vector<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector64", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector64<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector128", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector128<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector256", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector256<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector512", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector512<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Wasm.PackedSimd", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Arm.AdvSimd", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Sse", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Ssse3", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector128<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Avx512", StringComparison.Ordinal))
        {
            if (subjectIdPrefix.Contains("+VL::", StringComparison.Ordinal))
            {
                return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector128<", StringComparison.Ordinal) ||
                       returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector256<", StringComparison.Ordinal);
            }

            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector512<", StringComparison.Ordinal);
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Avx", StringComparison.Ordinal))
        {
            return returnManagedVectorType.StartsWith("System.Runtime.Intrinsics.Vector256<", StringComparison.Ordinal);
        }

        return false;
    }
}
