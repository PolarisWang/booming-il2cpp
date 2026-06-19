using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{

    private static bool TryCreateFixedMemoryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryExtractIntrinsicFactoryShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var methodName,
                out var returnManagedVectorType,
                out _,
                out var parameterSignature))
        {
            return false;
        }

        var methodBaseName = GetMethodBaseName(methodName);
        var parameters = SplitTopLevelParameters(parameterSignature);

        if (methodBaseName is "LoadVector512" or "LoadAlignedVector512" or "LoadAlignedVector512NonTemporal")
        {
            if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out var returnCarrierCppType, out _) ||
                parameters.Count != 1 ||
                !TryMapPointerParameterToCppType(parameters[0], out var pointerCppType))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-memory",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [pointerCppType], returnCarrierCppType),
                $"{returnCarrierCppType}{{}}",
                out plan);
        }

        if (methodBaseName is "Store" or "StoreAligned" or "StoreAlignedNonTemporal")
        {
            if (parameters.Count != 2 ||
                !TryMapPointerParameterToCppType(parameters[0], out var pointerCppType) ||
                !TryResolveCarrierFromManagedVectorType(parameters[1], out _, out var valueCarrierCppType, out _))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-memory",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [pointerCppType, valueCarrierCppType], "CHAOS_IL2CPP_INT32"),
                "CHAOS_BRIDGE_STATUS_OK",
                out plan);
        }

        if (methodBaseName is "BroadcastVector128ToVector512" or "BroadcastVector256ToVector512")
        {
            if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out var returnCarrierCppType, out _) ||
                parameters.Count != 1 ||
                !TryMapPointerParameterToCppType(parameters[0], out var pointerCppType))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-compose",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [pointerCppType], returnCarrierCppType),
                $"{returnCarrierCppType}{{}}",
                out plan);
        }

        if (methodBaseName == "BroadcastScalarToVector512")
        {
            if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out var returnCarrierCppType, out _) ||
                parameters.Count != 1 ||
                !TryResolveCarrierFromManagedVectorType(parameters[0], out _, out var inputCarrierCppType, out _))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-compose",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [inputCarrierCppType], returnCarrierCppType),
                $"{returnCarrierCppType}{{}}",
                out plan);
        }

        return false;
    }



    private static bool TryCreateFixedComposePlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryExtractIntrinsicFactoryShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var methodName,
                out var returnManagedVectorType,
                out _,
                out var parameterSignature))
        {
            return false;
        }

        var methodBaseName = GetMethodBaseName(methodName);
        var parameters = SplitTopLevelParameters(parameterSignature);

        if (methodBaseName is "ExtractVector128" or "ExtractVector256")
        {
            if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out var returnCarrierCppType, out _) ||
                parameters.Count != 2 ||
                !TryResolveCarrierFromManagedVectorType(parameters[0], out _, out var sourceCarrierCppType, out _) ||
                !string.Equals(parameters[1], "System.Byte", StringComparison.Ordinal))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-compose",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [sourceCarrierCppType, "CHAOS_IL2CPP_UINT8"], returnCarrierCppType),
                $"{returnCarrierCppType}{{}}",
                out plan);
        }

        if (methodBaseName is "InsertVector128" or "InsertVector256")
        {
            if (!TryResolveCarrierFromManagedVectorType(returnManagedVectorType, out _, out var returnCarrierCppType, out _) ||
                parameters.Count != 3 ||
                !TryResolveCarrierFromManagedVectorType(parameters[0], out _, out var baseCarrierCppType, out _) ||
                !TryResolveCarrierFromManagedVectorType(parameters[1], out _, out var insertedCarrierCppType, out _) ||
                !string.Equals(parameters[2], "System.Byte", StringComparison.Ordinal))
            {
                return false;
            }

            return TryCreatePlanCore(
                descriptor,
                "vector-fixed-compose",
                backend,
                new RuntimeSkeletonVectorKernelCarrierSchema(null, [baseCarrierCppType, insertedCarrierCppType, "CHAOS_IL2CPP_UINT8"], returnCarrierCppType),
                $"{returnCarrierCppType}{{}}",
                out plan);
        }

        return false;
    }



    private static bool TryCreateFixedShiftPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (TryExtractIntrinsicBinaryVectorVariableShiftOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var variableShiftMethodName,
                out var variableShiftScalarManagedType,
                out var variableShiftCountScalarManagedType,
                out var variableShiftCarrierCppType) &&
            TryResolveShiftTrait(variableShiftScalarManagedType, out var variableShiftTraitKind) &&
            TryResolveShiftScalarSelector(
                descriptor.SubjectIdPrefix,
                subjectId,
                variableShiftScalarManagedType,
                out var variableShiftScalarTypeSubjectId,
                out var variableShiftScalarResolutionKind,
                out var variableShiftScalarGenericArgumentIndex) &&
            TryMapManagedScalarCppType(variableShiftScalarManagedType, out var variableShiftScalarCppType) &&
            string.Equals(variableShiftCountScalarManagedType, ResolveUnsignedShiftCountManagedType(variableShiftScalarManagedType), StringComparison.Ordinal))
        {
            return variableShiftMethodName switch
            {
                "ShiftLeftLogicalVariable" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    variableShiftCarrierCppType,
                    variableShiftCarrierCppType,
                    variableShiftCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedShiftLeftVariable<{variableShiftScalarCppType}, {variableShiftCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-shift",
                    variableShiftTraitKind,
                    "shift-left",
                    variableShiftScalarTypeSubjectId,
                    GetFixedVectorWidthBytesFromCarrier(variableShiftCarrierCppType),
                    variableShiftScalarResolutionKind,
                    variableShiftScalarGenericArgumentIndex,
                    "request->arg1",
                    out plan),
                "ShiftRightLogicalVariable" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    variableShiftCarrierCppType,
                    variableShiftCarrierCppType,
                    variableShiftCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedShiftRightLogicalVariable<{variableShiftScalarCppType}, {variableShiftCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-shift",
                    variableShiftTraitKind,
                    "shift-right-logical",
                    variableShiftScalarTypeSubjectId,
                    GetFixedVectorWidthBytesFromCarrier(variableShiftCarrierCppType),
                    variableShiftScalarResolutionKind,
                    variableShiftScalarGenericArgumentIndex,
                    "request->arg1",
                    out plan),
                "ShiftRightArithmeticVariable"
                    when variableShiftTraitKind is RuntimeSkeletonVectorShiftTraitKind.Signed
                        or RuntimeSkeletonVectorShiftTraitKind.NativeSigned
                        or RuntimeSkeletonVectorShiftTraitKind.Deferred =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        variableShiftCarrierCppType,
                        variableShiftCarrierCppType,
                        variableShiftCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedShiftRightArithmeticVariable<{variableShiftScalarCppType}, {variableShiftCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-shift",
                        variableShiftTraitKind,
                        "shift-right-arithmetic",
                        variableShiftScalarTypeSubjectId,
                        GetFixedVectorWidthBytesFromCarrier(variableShiftCarrierCppType),
                        variableShiftScalarResolutionKind,
                        variableShiftScalarGenericArgumentIndex,
                        "request->arg1",
                        out plan),
                _ => false,
            };
        }

        if (!TryExtractIntrinsicShiftShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var normalizedMethodName,
                out var scalarManagedType,
                out var carrierCppType,
                out var fixedVectorWidthBytes,
                out var shiftCountCppType,
                out var shiftCountValueExpression) ||
            !TryResolveShiftTrait(scalarManagedType, out var shiftTraitKind) ||
            !TryResolveShiftScalarSelector(
                descriptor.SubjectIdPrefix,
                subjectId,
                scalarManagedType,
                out var scalarTypeSubjectId,
                out var scalarResolutionKind,
                out var scalarGenericArgumentIndex))
        {
            return false;
        }

        return normalizedMethodName switch
        {
            var current when string.Equals(current, "ShiftLeft", StringComparison.Ordinal) ||
                              string.Equals(current, "op_LeftShift", StringComparison.Ordinal) =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    carrierCppType,
                    shiftCountCppType,
                    carrierCppType,
                    string.Empty,
                    "vector-fixed-shift",
                    shiftTraitKind,
                    "shift-left",
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    shiftCountValueExpression,
                    out plan),
            var current when string.Equals(current, "ShiftRightLogical", StringComparison.Ordinal) ||
                              string.Equals(current, "op_UnsignedRightShift", StringComparison.Ordinal) =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    carrierCppType,
                    shiftCountCppType,
                    carrierCppType,
                    string.Empty,
                    "vector-fixed-shift",
                    shiftTraitKind,
                    "shift-right-logical",
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    shiftCountValueExpression,
                    out plan),
            var current when string.Equals(current, "ShiftRightArithmetic", StringComparison.Ordinal) =>
                shiftTraitKind is RuntimeSkeletonVectorShiftTraitKind.Signed
                    or RuntimeSkeletonVectorShiftTraitKind.NativeSigned
                    or RuntimeSkeletonVectorShiftTraitKind.Deferred
                    ? TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        carrierCppType,
                        shiftCountCppType,
                        carrierCppType,
                        string.Empty,
                        "vector-fixed-shift",
                        shiftTraitKind,
                        "shift-right-arithmetic",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        shiftCountValueExpression,
                        out plan)
                    : false,
            var current when string.Equals(current, "op_RightShift", StringComparison.Ordinal) =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    carrierCppType,
                    shiftCountCppType,
                    carrierCppType,
                    string.Empty,
                    "vector-fixed-shift",
                    shiftTraitKind,
                    shiftTraitKind switch
                    {
                        RuntimeSkeletonVectorShiftTraitKind.Signed or RuntimeSkeletonVectorShiftTraitKind.NativeSigned =>
                            "shift-right-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.Unsigned or RuntimeSkeletonVectorShiftTraitKind.NativeUnsigned =>
                            "shift-right-logical",
                        RuntimeSkeletonVectorShiftTraitKind.Floating or RuntimeSkeletonVectorShiftTraitKind.Deferred =>
                            "shift-right-dynamic",
                        _ => "shift-right-dynamic",
                    },
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    shiftCountValueExpression,
                    out plan),
            _ => false,
        };
    }



    private static bool TryExtractIntrinsicShiftShape(
        string subjectIdPrefix,
        string subjectId,
        out string normalizedMethodName,
        out string scalarManagedType,
        out string carrierCppType,
        out int fixedVectorWidthBytes,
        out string shiftCountCppType,
        out string shiftCountValueExpression)
    {
        normalizedMethodName = string.Empty;
        scalarManagedType = string.Empty;
        carrierCppType = string.Empty;
        fixedVectorWidthBytes = 0;
        shiftCountCppType = "CHAOS_IL2CPP_INT32";
        shiftCountValueExpression = "request->arg1";

        if (subjectIdPrefix.StartsWith("/System.Numerics.Vector", StringComparison.Ordinal))
        {
            if (subjectIdPrefix.EndsWith("::", StringComparison.Ordinal))
            {
                if (!TryExtractStaticMethodShape(
                        subjectIdPrefix,
                        subjectId,
                        out var methodName,
                        out var returnType,
                        out var parameterSignature) ||
                    !TryResolveCarrierFromManagedVectorType(
                        returnType,
                        out scalarManagedType,
                        out carrierCppType,
                        out fixedVectorWidthBytes))
                {
                    return false;
                }

                var parameters = SplitTopLevelParameters(parameterSignature);
                if (parameters.Count != 2 ||
                    !string.Equals(parameters[0], returnType, StringComparison.Ordinal) ||
                    !TryResolveShiftCountShape(parameters[1], out shiftCountCppType, out shiftCountValueExpression))
                {
                    return false;
                }

                normalizedMethodName = GetMethodBaseName(methodName);
                return true;
            }

            if (TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out scalarManagedType, out var numericsMethodSuffix))
            {
                if (!TryGetFixedVectorWidthBytes(subjectIdPrefix, out fixedVectorWidthBytes))
                {
                    return false;
                }

                carrierCppType = GetCarrierCppTypeForFixedVectorWidth(fixedVectorWidthBytes);
                normalizedMethodName = numericsMethodSuffix.Split(':', 2, StringSplitOptions.None)[0];
                return true;
            }

            if (TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out numericsMethodSuffix))
            {
                if (!TryGetFixedVectorWidthBytes(subjectIdPrefix, out fixedVectorWidthBytes))
                {
                    return false;
                }

                carrierCppType = GetCarrierCppTypeForFixedVectorWidth(fixedVectorWidthBytes);
                normalizedMethodName = numericsMethodSuffix.Split(':', 2, StringSplitOptions.None)[0];
                scalarManagedType = subjectId.Contains("!!", StringComparison.Ordinal) ? "!!0" : "!0";
                return true;
            }

            return false;
        }

        if (subjectIdPrefix.EndsWith("::", StringComparison.Ordinal))
        {
            if (!TryExtractStaticMethodShape(
                    subjectIdPrefix,
                    subjectId,
                    out var methodName,
                    out var returnType,
                    out var parameterSignature))
            {
                return false;
            }

            if (!TryResolveCarrierFromManagedVectorType(
                    returnType,
                    out scalarManagedType,
                    out carrierCppType,
                    out fixedVectorWidthBytes))
            {
                return false;
            }

            var parameters = SplitTopLevelParameters(parameterSignature);
            if (parameters.Count != 2 ||
                !string.Equals(parameters[0], returnType, StringComparison.Ordinal) ||
                !TryResolveShiftCountShape(parameters[1], out shiftCountCppType, out shiftCountValueExpression))
            {
                return false;
            }

            normalizedMethodName = GetMethodBaseName(methodName);
            return true;
        }

        if (TryExtractClosedIntrinsicScalarManagedType(subjectIdPrefix, subjectId, out scalarManagedType, out var methodSuffix))
        {
            if (!TryGetFixedVectorWidthBytes(subjectIdPrefix, out fixedVectorWidthBytes))
            {
                return false;
            }

            carrierCppType = GetCarrierCppTypeForFixedVectorWidth(fixedVectorWidthBytes);
            normalizedMethodName = methodSuffix.Split(':', 2, StringSplitOptions.None)[0];
            return true;
        }

        if (TryExtractOpenIntrinsicMethodSuffix(subjectIdPrefix, subjectId, out methodSuffix))
        {
            if (!TryGetFixedVectorWidthBytes(subjectIdPrefix, out fixedVectorWidthBytes))
            {
                return false;
            }

            carrierCppType = GetCarrierCppTypeForFixedVectorWidth(fixedVectorWidthBytes);
            normalizedMethodName = methodSuffix.Split(':', 2, StringSplitOptions.None)[0];
            scalarManagedType = subjectId.Contains("!!", StringComparison.Ordinal) ? "!!0" : "!0";
            return true;
        }

        return false;
    }



    private static bool TryExtractStaticMethodShape(
        string subjectIdPrefix,
        string subjectId,
        out string methodName,
        out string returnType,
        out string parameterSignature)
    {
        methodName = string.Empty;
        returnType = string.Empty;
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
        var openParenIndex = returnAndParameters.IndexOf('(');
        if (openParenIndex <= 0)
        {
            return false;
        }

        returnType = returnAndParameters[..openParenIndex];
        var parameterStart = openParenIndex + 1;
        var parameterEnd = returnAndParameters.LastIndexOf(')');
        if (parameterEnd < parameterStart)
        {
            return false;
        }

        parameterSignature = returnAndParameters[parameterStart..parameterEnd];
        return true;
    }



    private static bool TryResolveCarrierFromManagedVectorType(
        string managedVectorType,
        out string scalarManagedType,
        out string carrierCppType,
        out int fixedVectorWidthBytes)
    {
        scalarManagedType = string.Empty;
        carrierCppType = string.Empty;
        fixedVectorWidthBytes = 0;

        foreach (var (prefix, widthBytes) in new[]
                 {
                     ("System.Numerics.Vector<", 32),
                     ("System.Runtime.Intrinsics.Vector64<", 8),
                     ("System.Runtime.Intrinsics.Vector128<", 16),
                     ("System.Runtime.Intrinsics.Vector256<", 32),
                     ("System.Runtime.Intrinsics.Vector512<", 64),
                 })
        {
            if (!managedVectorType.StartsWith(prefix, StringComparison.Ordinal) ||
                !managedVectorType.EndsWith(">", StringComparison.Ordinal))
            {
                continue;
            }

            scalarManagedType = managedVectorType[prefix.Length..^1];
            fixedVectorWidthBytes = widthBytes;
            carrierCppType = GetCarrierCppTypeForFixedVectorWidth(widthBytes);
            return true;
        }

        return false;
    }

}
