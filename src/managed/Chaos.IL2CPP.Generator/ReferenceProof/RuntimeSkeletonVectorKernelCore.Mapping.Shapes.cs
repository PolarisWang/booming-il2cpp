using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{

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
