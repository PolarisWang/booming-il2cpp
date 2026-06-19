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

    private static bool TryExtractIntrinsicUnaryVectorOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string inputScalarManagedType,
        out string returnScalarManagedType,
        out string parameterCarrierCppType,
        out string returnCarrierCppType)
    {
        methodName = string.Empty;
        inputScalarManagedType = string.Empty;
        returnScalarManagedType = string.Empty;
        parameterCarrierCppType = string.Empty;
        returnCarrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 1 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out inputScalarManagedType, out parameterCarrierCppType, out var parameterWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out returnCarrierCppType, out var returnWidth))
        {
            return false;
        }

        return parameterWidth == returnWidth;
    }

    private static bool TryExtractIntrinsicBinaryVectorOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string leftScalarManagedType,
        out string rightScalarManagedType,
        out string returnScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        leftScalarManagedType = string.Empty;
        rightScalarManagedType = string.Empty;
        returnScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 2 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out leftScalarManagedType, out var leftCarrierCppType, out var leftWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out rightScalarManagedType, out var rightCarrierCppType, out var rightWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return leftWidth == rightWidth &&
               rightWidth == returnWidth &&
               string.Equals(leftCarrierCppType, rightCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(rightCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicTernaryVectorOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string firstScalarManagedType,
        out string secondScalarManagedType,
        out string thirdScalarManagedType,
        out string returnScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        firstScalarManagedType = string.Empty;
        secondScalarManagedType = string.Empty;
        thirdScalarManagedType = string.Empty;
        returnScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 3 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out firstScalarManagedType, out var firstCarrierCppType, out var firstWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out secondScalarManagedType, out var secondCarrierCppType, out var secondWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[2], out thirdScalarManagedType, out var thirdCarrierCppType, out var thirdWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return firstWidth == secondWidth &&
               secondWidth == thirdWidth &&
               thirdWidth == returnWidth &&
               string.Equals(firstCarrierCppType, secondCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(secondCarrierCppType, thirdCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(thirdCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicTernaryVectorByteImmediateOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string carrierCppType)
    {
        methodName = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out _,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 4 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out _, out var firstCarrierCppType, out var firstWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out _, out var secondCarrierCppType, out var secondWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[2], out _, out var thirdCarrierCppType, out var thirdWidth) ||
            !string.Equals(parameters[3], "System.Byte", StringComparison.Ordinal) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return firstWidth == secondWidth &&
               secondWidth == thirdWidth &&
               thirdWidth == returnWidth &&
               string.Equals(firstCarrierCppType, secondCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(secondCarrierCppType, thirdCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(thirdCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicUnaryVectorByteImmediateOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string scalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        scalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 2 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out scalarManagedType, out var inputCarrierCppType, out var inputWidth) ||
            !string.Equals(parameters[1], "System.Byte", StringComparison.Ordinal) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return inputWidth == returnWidth &&
               string.Equals(scalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(inputCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicUnaryVectorConversionShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string inputScalarManagedType,
        out string returnScalarManagedType,
        out string inputCarrierCppType,
        out string returnCarrierCppType)
    {
        methodName = string.Empty;
        inputScalarManagedType = string.Empty;
        returnScalarManagedType = string.Empty;
        inputCarrierCppType = string.Empty;
        returnCarrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out returnCarrierCppType, out _))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 1 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out inputScalarManagedType, out inputCarrierCppType, out _))
        {
            return false;
        }

        return true;
    }

    private static bool TryExtractIntrinsicBinaryVectorVariableShiftOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string valueScalarManagedType,
        out string shiftCountScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        valueScalarManagedType = string.Empty;
        shiftCountScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 2 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out valueScalarManagedType, out var valueCarrierCppType, out var valueWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out shiftCountScalarManagedType, out var shiftCarrierCppType, out var shiftWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return valueWidth == shiftWidth &&
               shiftWidth == returnWidth &&
               string.Equals(valueScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(valueCarrierCppType, shiftCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(shiftCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicTernaryVectorWideningByteImmediateOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string inputScalarManagedType,
        out string returnCarrierCppType,
        out string inputCarrierCppType)
    {
        methodName = string.Empty;
        inputScalarManagedType = string.Empty;
        returnCarrierCppType = string.Empty;
        inputCarrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out _,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 3 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out inputScalarManagedType, out var firstCarrierCppType, out var firstWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out var secondScalarManagedType, out var secondCarrierCppType, out var secondWidth) ||
            !string.Equals(parameters[2], "System.Byte", StringComparison.Ordinal) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out returnCarrierCppType, out var returnWidth))
        {
            return false;
        }

        inputCarrierCppType = firstCarrierCppType;
        return firstWidth == secondWidth &&
               string.Equals(inputScalarManagedType, secondScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(firstCarrierCppType, secondCarrierCppType, StringComparison.Ordinal) &&
               returnWidth == firstWidth;
    }

    private static bool TryExtractIntrinsicBinaryVectorByteImmediateOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string scalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        scalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 3 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out scalarManagedType, out var leftCarrierCppType, out var leftWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out var rightScalarManagedType, out var rightCarrierCppType, out var rightWidth) ||
            !string.Equals(parameters[2], "System.Byte", StringComparison.Ordinal) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return leftWidth == rightWidth &&
               rightWidth == returnWidth &&
               string.Equals(scalarManagedType, rightScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(scalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(leftCarrierCppType, rightCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(rightCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicBinaryRotateVariableOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string valueScalarManagedType,
        out string rotateCountScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        valueScalarManagedType = string.Empty;
        rotateCountScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 2 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out valueScalarManagedType, out var valueCarrierCppType, out var valueWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out rotateCountScalarManagedType, out var rotateCarrierCppType, out var rotateWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return valueWidth == rotateWidth &&
               rotateWidth == returnWidth &&
               string.Equals(valueScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(valueCarrierCppType, rotateCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(rotateCarrierCppType, carrierCppType, StringComparison.Ordinal) &&
               TryResolveRotateCountManagedType(valueScalarManagedType, out var expectedRotateCountScalarManagedType) &&
               string.Equals(rotateCountScalarManagedType, expectedRotateCountScalarManagedType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicBinaryVectorIndexOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string valueScalarManagedType,
        out string indexScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        valueScalarManagedType = string.Empty;
        indexScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 2 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out valueScalarManagedType, out var valueCarrierCppType, out var valueWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out indexScalarManagedType, out var indexCarrierCppType, out var indexWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return valueWidth == indexWidth &&
               indexWidth == returnWidth &&
               string.Equals(valueScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(valueCarrierCppType, indexCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(indexCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicTernaryVectorIndexedBlendOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string valueScalarManagedType,
        out string indexScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        valueScalarManagedType = string.Empty;
        indexScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 3 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out valueScalarManagedType, out var firstCarrierCppType, out var firstWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out indexScalarManagedType, out var secondCarrierCppType, out var secondWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[2], out var thirdScalarManagedType, out var thirdCarrierCppType, out var thirdWidth) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return firstWidth == secondWidth &&
               secondWidth == thirdWidth &&
               thirdWidth == returnWidth &&
               string.Equals(valueScalarManagedType, thirdScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(valueScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(firstCarrierCppType, secondCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(secondCarrierCppType, thirdCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(thirdCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static bool TryExtractIntrinsicTernaryVectorMaskByteImmediateOperationShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string valueScalarManagedType,
        out string maskScalarManagedType,
        out string carrierCppType)
    {
        methodName = string.Empty;
        valueScalarManagedType = string.Empty;
        maskScalarManagedType = string.Empty;
        carrierCppType = string.Empty;

        if (!TryExtractIntrinsicFactoryShape(
                subjectIdPrefix,
                subjectId,
                out methodName,
                out var returnManagedVectorType,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count != 4 ||
            !TryResolveCarrierFromManagedVectorType(parameters[0], out valueScalarManagedType, out var firstCarrierCppType, out var firstWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[1], out var secondScalarManagedType, out var secondCarrierCppType, out var secondWidth) ||
            !TryResolveCarrierFromManagedVectorType(parameters[2], out maskScalarManagedType, out var thirdCarrierCppType, out var thirdWidth) ||
            !string.Equals(parameters[3], "System.Byte", StringComparison.Ordinal) ||
            !TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out carrierCppType, out var returnWidth))
        {
            return false;
        }

        return firstWidth == secondWidth &&
               secondWidth == thirdWidth &&
               thirdWidth == returnWidth &&
               string.Equals(valueScalarManagedType, secondScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(valueScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
               string.Equals(firstCarrierCppType, secondCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(secondCarrierCppType, thirdCarrierCppType, StringComparison.Ordinal) &&
               string.Equals(thirdCarrierCppType, carrierCppType, StringComparison.Ordinal);
    }

    private static string GetIntrinsicManagedVectorTypeName(string subjectIdPrefix)
    {
        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector64", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector64";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector128", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector128";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector256", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector256";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Vector512", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector512";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Wasm.PackedSimd", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.Arm.AdvSimd", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Sse", StringComparison.Ordinal) ||
            subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Ssse3", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector128";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Avx512", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector512";
        }

        if (subjectIdPrefix.StartsWith("/System.Runtime.Intrinsics.X86.Avx", StringComparison.Ordinal))
        {
            return "System.Runtime.Intrinsics.Vector256";
        }

        throw new InvalidOperationException($"unsupported intrinsic vector type prefix '{subjectIdPrefix}'");
    }

    private static string GetFixedVectorManagedTypeName(string subjectIdPrefix)
    {
        if (subjectIdPrefix.StartsWith("/System.Numerics.Vector", StringComparison.Ordinal))
        {
            return "System.Numerics.Vector";
        }

        return GetIntrinsicManagedVectorTypeName(subjectIdPrefix);
    }

    private static string GetMethodBaseName(string methodName)
    {
        var genericArgumentStart = methodName.IndexOf('<');
        var genericArityStart = methodName.IndexOf('`');
        var cutIndex = genericArgumentStart >= 0 && genericArityStart >= 0
            ? Math.Min(genericArgumentStart, genericArityStart)
            : genericArgumentStart >= 0
                ? genericArgumentStart
                : genericArityStart;
        return cutIndex >= 0
            ? methodName[..cutIndex]
            : methodName;
    }

    private static bool TryParseGenericArgumentSelector(
        string managedType,
        out RuntimeSkeletonVectorKernelScalarResolutionKind scalarResolutionKind,
        out int genericArgumentIndex)
    {
        scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ConcreteTypeToken;
        genericArgumentIndex = -1;

        if (managedType.StartsWith("!!", StringComparison.Ordinal) &&
            int.TryParse(managedType[2..], out var methodArgumentIndex) &&
            methodArgumentIndex >= 0)
        {
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.MethodGenericArgument;
            genericArgumentIndex = methodArgumentIndex;
            return true;
        }

        if (managedType.StartsWith("!", StringComparison.Ordinal) &&
            int.TryParse(managedType[1..], out var classArgumentIndex) &&
            classArgumentIndex >= 0)
        {
            scalarResolutionKind = RuntimeSkeletonVectorKernelScalarResolutionKind.ClassGenericArgument;
            genericArgumentIndex = classArgumentIndex;
            return true;
        }

        return false;
    }

    private static bool TryResolveRotateCountManagedType(string valueScalarManagedType, out string rotateCountScalarManagedType)
    {
        rotateCountScalarManagedType = valueScalarManagedType switch
        {
            "System.Int32" or "System.UInt32" => "System.UInt32",
            "System.Int64" or "System.UInt64" => "System.UInt64",
            _ => string.Empty,
        };
        return !string.IsNullOrWhiteSpace(rotateCountScalarManagedType);
    }

    private static string ResolveUnsignedShiftCountManagedType(string valueScalarManagedType) =>
        valueScalarManagedType switch
        {
            "System.SByte" or "System.Byte" => "System.Byte",
            "System.Int16" or "System.UInt16" => "System.UInt16",
            "System.Int32" or "System.UInt32" => "System.UInt32",
            "System.Int64" or "System.UInt64" => "System.UInt64",
            _ => string.Empty,
        };

    private static bool TryGetFixedVectorWidthBytes(string subjectIdPrefix, out int widthBytes)
    {
        widthBytes = subjectIdPrefix switch
        {
            "/System.Numerics.Vector::" => 32,
            "/System.Numerics.Vector<" => 32,
            "/System.Numerics.Vector`1::" => 32,
            "/System.Runtime.Intrinsics.Vector64::" => 8,
            "/System.Runtime.Intrinsics.Vector64<" => 8,
            "/System.Runtime.Intrinsics.Vector64`1::" => 8,
            "/System.Runtime.Intrinsics.Vector128::" => 16,
            "/System.Runtime.Intrinsics.Vector128<" => 16,
            "/System.Runtime.Intrinsics.Vector128`1::" => 16,
            "/System.Runtime.Intrinsics.Vector256::" => 32,
            "/System.Runtime.Intrinsics.Vector256<" => 32,
            "/System.Runtime.Intrinsics.Vector256`1::" => 32,
            "/System.Runtime.Intrinsics.Vector512::" => 64,
            "/System.Runtime.Intrinsics.Vector512<" => 64,
            "/System.Runtime.Intrinsics.Vector512`1::" => 64,
            "/System.Runtime.Intrinsics.Wasm.PackedSimd::" => 16,
            "/System.Runtime.Intrinsics.Arm.AdvSimd::" => 16,
            "/System.Runtime.Intrinsics.Arm.AdvSimd+Arm64::" => 16,
            "/System.Runtime.Intrinsics.X86.Sse::" => 16,
            "/System.Runtime.Intrinsics.X86.Sse2::" => 16,
            "/System.Runtime.Intrinsics.X86.Ssse3::" => 16,
            "/System.Runtime.Intrinsics.X86.Sse41::" => 16,
            "/System.Runtime.Intrinsics.X86.Sse42::" => 16,
            "/System.Runtime.Intrinsics.X86.Avx::" => 32,
            "/System.Runtime.Intrinsics.X86.Avx2::" => 32,
            "/System.Runtime.Intrinsics.X86.Avx512BW::" => 64,
            "/System.Runtime.Intrinsics.X86.Avx512BW+VL::" => 32,
            "/System.Runtime.Intrinsics.X86.Avx512DQ::" => 64,
            "/System.Runtime.Intrinsics.X86.Avx512F::" => 64,
            "/System.Runtime.Intrinsics.X86.Avx512F+VL::" => 32,
            _ => 0,
        };
        return widthBytes != 0;
    }

    private static string GetHelperNamespace() => "chaos::il2cpp::runtime_core";
}
