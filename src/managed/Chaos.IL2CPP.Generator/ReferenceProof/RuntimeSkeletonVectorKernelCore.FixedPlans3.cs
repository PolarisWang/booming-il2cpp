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



    private static bool TryCreateFixedBitwisePlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (TryExtractIntrinsicUnaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var unaryMethodName,
                out _,
                out _,
                out _,
                out var unaryCarrierCppType))
        {
            if (unaryMethodName == "Not")
            {
                return TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryCarrierCppType,
                    unaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedOnesComplement<{unaryCarrierCppType}>(request->arg0)",
                    "vector-fixed-bitwise",
                    out plan);
            }
        }

        if (TryExtractIntrinsicBinaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var binaryMethodName,
                out var leftScalarManagedType,
                out var rightScalarManagedType,
                out var returnScalarManagedType,
                out var binaryCarrierCppType))
        {
            if (binaryMethodName is "RotateLeftVariable" or "RotateRightVariable")
            {
                if (!string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) ||
                    !TryResolveRotateCountManagedType(leftScalarManagedType, out var expectedRotateCountManagedType) ||
                    !string.Equals(rightScalarManagedType, expectedRotateCountManagedType, StringComparison.Ordinal) ||
                    !TryMapManagedScalarCppType(leftScalarManagedType, out var rotateValueScalarCppType) ||
                    !TryMapManagedScalarCppType(rightScalarManagedType, out var rotateCountScalarCppType))
                {
                    return false;
                }

                return binaryMethodName switch
                {
                    "RotateLeftVariable" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedRotateLeftVariable<{rotateValueScalarCppType}, {rotateCountScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    "RotateRightVariable" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedRotateRightVariable<{rotateValueScalarCppType}, {rotateCountScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    _ => false,
                };
            }

            if (binaryMethodName is "And" or "Or" or "Xor" or "AndNot")
            {
                return binaryMethodName switch
                {
                    "And" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedBitwiseAnd<{binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    "Or" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedBitwiseOr<{binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    "Xor" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedBitwiseXor<{binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    "AndNot" => TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedBitwiseAndNot<{binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                    _ => false,
                };
            }
        }

        if (TryExtractIntrinsicTernaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var ternaryMethodName,
                out _,
                out _,
                out _,
                out _,
                out var ternaryCarrierCppType))
        {
            if (ternaryMethodName == "BitwiseSelect")
            {
                return TryCreateStaticTernaryCustomArgsPlan(
                    descriptor,
                    backend,
                    ternaryCarrierCppType,
                    ternaryCarrierCppType,
                    ternaryCarrierCppType,
                    ternaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseSelect<{ternaryCarrierCppType}>(request->arg0, request->arg1, request->arg2)",
                    "vector-fixed-bitwise",
                    out plan);
            }
        }

        if (TryExtractIntrinsicTernaryVectorByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var ternaryImmediateMethodName,
                out var ternaryImmediateCarrierCppType))
        {
            if (ternaryImmediateMethodName == "TernaryLogic")
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [ternaryImmediateCarrierCppType, ternaryImmediateCarrierCppType, ternaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        ternaryImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedTernaryLogic<{ternaryImmediateCarrierCppType}>(request->arg0, request->arg1, request->arg2, request->arg3)",
                    out plan);
            }
        }

        if (TryExtractIntrinsicTernaryVectorWideningByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var wideningImmediateMethodName,
                out var wideningImmediateInputScalarManagedType,
                out var wideningImmediateReturnCarrierCppType,
                out var wideningImmediateInputCarrierCppType) &&
            TryMapManagedScalarCppType(wideningImmediateInputScalarManagedType, out _))
        {
            if (wideningImmediateMethodName == "SumAbsoluteDifferencesInBlock32" &&
                string.Equals(wideningImmediateInputScalarManagedType, "System.Byte", StringComparison.Ordinal))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [wideningImmediateInputCarrierCppType, wideningImmediateInputCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        wideningImmediateReturnCarrierCppType),
                    $"{wideningImmediateReturnCarrierCppType}{{}}",
                    out plan);
            }
        }

        if (TryExtractIntrinsicUnaryVectorByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var unaryImmediateMethodName,
                out var unaryImmediateScalarManagedType,
                out var unaryImmediateCarrierCppType) &&
            TryMapManagedScalarCppType(unaryImmediateScalarManagedType, out var unaryImmediateScalarCppType))
        {
            if (unaryImmediateMethodName is "RotateLeft" or "RotateRight")
            {
                return unaryImmediateMethodName switch
                {
                    "RotateLeft" => TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(
                            null,
                            [unaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                            unaryImmediateCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedRotateLeft<{unaryImmediateScalarCppType}, {unaryImmediateCarrierCppType}>(request->arg0, request->arg1)",
                        out plan),
                    "RotateRight" => TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(
                            null,
                            [unaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                            unaryImmediateCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedRotateRight<{unaryImmediateScalarCppType}, {unaryImmediateCarrierCppType}>(request->arg0, request->arg1)",
                        out plan),
                    _ => false,
                };
            }
        }

        if (TryExtractIntrinsicUnaryVectorConversionShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var conversionMethodName,
                out var conversionInputScalarManagedType,
                out var conversionReturnScalarManagedType,
                out var conversionInputCarrierCppType,
                out var conversionReturnCarrierCppType) &&
            TryMapManagedScalarCppType(conversionInputScalarManagedType, out var conversionInputScalarCppType) &&
            TryMapManagedScalarCppType(conversionReturnScalarManagedType, out var conversionReturnScalarCppType))
        {
            return conversionMethodName switch
            {
                "ConvertToVector128Byte"
                    when string.Equals(conversionReturnScalarManagedType, "System.Byte", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int16" or "System.UInt16" or "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128SByte"
                    when string.Equals(conversionReturnScalarManagedType, "System.SByte", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int16" or "System.UInt16" or "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Int16"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int16", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128UInt16"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt16", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Int32"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int64" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128UInt32"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int64" or "System.UInt64" or "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector256Double"
                    when string.Equals(conversionReturnScalarManagedType, "System.Double", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt32" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector256Single"
                    when string.Equals(conversionReturnScalarManagedType, "System.Single", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt32" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector256UInt32"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Int64"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int64", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is
                             "System.Byte" or
                             "System.SByte" or
                             "System.Int16" or
                             "System.UInt16" or
                             "System.Int32" or
                             "System.UInt32" or
                             "System.Single" or
                             "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512UInt64"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt64", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is
                             "System.Byte" or
                             "System.SByte" or
                             "System.Int16" or
                             "System.UInt16" or
                             "System.Int32" or
                             "System.UInt32" or
                             "System.Single" or
                             "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Int32"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int32", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is
                             "System.Byte" or
                             "System.SByte" or
                             "System.Int16" or
                             "System.UInt16" or
                             "System.Int64" or
                             "System.UInt64" or
                             "System.Single" or
                             "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512UInt32"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is
                             "System.Byte" or
                             "System.SByte" or
                             "System.Int16" or
                             "System.UInt16" or
                             "System.Int64" or
                             "System.UInt64" or
                             "System.Single" or
                             "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Double"
                    when string.Equals(conversionReturnScalarManagedType, "System.Double", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is
                             "System.Int32" or
                             "System.UInt32" or
                             "System.Int64" or
                             "System.UInt64" or
                             "System.Single" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Int16"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int16", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Byte" or "System.SByte" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512UInt16"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt16", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Byte" or "System.SByte" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128ByteWithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Byte", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt16" or "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128SByteWithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.SByte", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int16" or "System.Int32" or "System.Int64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Int16WithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int16", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int32" or "System.Int64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128UInt16WithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt16", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt32" or "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Int32WithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128UInt32WithSaturation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt64" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorSaturating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector256UInt32WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128UInt32WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Int64WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int64", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512UInt64WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt64", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Int32WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Int32", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512UInt32WithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.UInt32", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" or "System.Double" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512DoubleWithTruncation"
                    when string.Equals(conversionReturnScalarManagedType, "System.Double", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Single" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVectorTruncating<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector512Single"
                    when string.Equals(conversionReturnScalarManagedType, "System.Single", StringComparison.Ordinal) &&
                         string.Equals(conversionReturnCarrierCppType, "RuntimeIntrinsicVector512Carrier", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.Int32" or "System.UInt32" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Single"
                    when string.Equals(conversionReturnScalarManagedType, "System.Single", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt32" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                "ConvertToVector128Double"
                    when string.Equals(conversionReturnScalarManagedType, "System.Double", StringComparison.Ordinal) &&
                         conversionInputScalarManagedType is "System.UInt32" =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [conversionInputCarrierCppType], conversionReturnCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedConvertToVector<{conversionReturnScalarCppType}, {conversionInputScalarCppType}, {conversionReturnCarrierCppType}, {conversionInputCarrierCppType}>(request->arg0)",
                        out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicUnaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var unaryNumericMethodName,
                out var unaryNumericInputScalarManagedType,
                out var unaryNumericReturnScalarManagedType,
                out var unaryNumericParameterCppType,
                out var unaryNumericReturnCarrierCppType) &&
            string.Equals(unaryNumericInputScalarManagedType, unaryNumericReturnScalarManagedType, StringComparison.Ordinal) &&
            TryMapManagedScalarCppType(unaryNumericInputScalarManagedType, out var unaryNumericScalarCppType) &&
            (string.Equals(unaryNumericInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
             string.Equals(unaryNumericInputScalarManagedType, "System.Double", StringComparison.Ordinal)))
        {
            return unaryNumericMethodName switch
            {
                "GetExponent" => TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryNumericParameterCppType,
                    unaryNumericReturnCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedGetExponent<{unaryNumericScalarCppType}, {unaryNumericReturnCarrierCppType}>(request->arg0)",
                    "vector-fixed-bitwise",
                    out plan),
                "Reciprocal14" => TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryNumericParameterCppType,
                    unaryNumericReturnCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedReciprocal14<{unaryNumericScalarCppType}, {unaryNumericReturnCarrierCppType}>(request->arg0)",
                    "vector-fixed-bitwise",
                    out plan),
                "ReciprocalSqrt14" => TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryNumericParameterCppType,
                    unaryNumericReturnCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedReciprocalSqrt14<{unaryNumericScalarCppType}, {unaryNumericReturnCarrierCppType}>(request->arg0)",
                    "vector-fixed-bitwise",
                    out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicUnaryVectorByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var unaryNumericImmediateMethodName,
                out var unaryNumericImmediateScalarManagedType,
                out var unaryNumericImmediateCarrierCppType) &&
            TryMapManagedScalarCppType(unaryNumericImmediateScalarManagedType, out var unaryNumericImmediateScalarCppType) &&
            (string.Equals(unaryNumericImmediateScalarManagedType, "System.Single", StringComparison.Ordinal) ||
             string.Equals(unaryNumericImmediateScalarManagedType, "System.Double", StringComparison.Ordinal)))
        {
            return unaryNumericImmediateMethodName switch
            {
                "GetMantissa" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [unaryNumericImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        unaryNumericImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedGetMantissa<{unaryNumericImmediateScalarCppType}, {unaryNumericImmediateCarrierCppType}>(request->arg0, request->arg1)",
                    out plan),
                "RoundScale" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [unaryNumericImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        unaryNumericImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedRoundScale<{unaryNumericImmediateScalarCppType}, {unaryNumericImmediateCarrierCppType}>(request->arg0, request->arg1)",
                    out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicBinaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var binaryNumericMethodName,
                out var binaryNumericLeftScalarManagedType,
                out var binaryNumericRightScalarManagedType,
                out var binaryNumericReturnScalarManagedType,
                out var binaryNumericCarrierCppType) &&
            string.Equals(binaryNumericLeftScalarManagedType, binaryNumericRightScalarManagedType, StringComparison.Ordinal) &&
            string.Equals(binaryNumericLeftScalarManagedType, binaryNumericReturnScalarManagedType, StringComparison.Ordinal) &&
            TryMapManagedScalarCppType(binaryNumericLeftScalarManagedType, out var binaryNumericScalarCppType) &&
            (string.Equals(binaryNumericLeftScalarManagedType, "System.Single", StringComparison.Ordinal) ||
             string.Equals(binaryNumericLeftScalarManagedType, "System.Double", StringComparison.Ordinal)))
        {
            return binaryNumericMethodName switch
            {
                "Scale" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryNumericCarrierCppType,
                    binaryNumericCarrierCppType,
                    binaryNumericCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedScale<{binaryNumericScalarCppType}, {binaryNumericCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-bitwise",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicTernaryVectorMaskByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var fixupMethodName,
                out var fixupValueScalarManagedType,
                out var fixupMaskScalarManagedType,
                out var fixupCarrierCppType) &&
            TryMapManagedScalarCppType(fixupValueScalarManagedType, out var fixupValueScalarCppType) &&
            TryMapManagedScalarCppType(fixupMaskScalarManagedType, out var fixupMaskScalarCppType) &&
            (string.Equals(fixupValueScalarManagedType, "System.Single", StringComparison.Ordinal) ||
             string.Equals(fixupValueScalarManagedType, "System.Double", StringComparison.Ordinal)))
        {
            return fixupMethodName switch
            {
                "Fixup" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [fixupCarrierCppType, fixupCarrierCppType, fixupCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        fixupCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedFixup<{fixupValueScalarCppType}, {fixupMaskScalarCppType}, {fixupCarrierCppType}>(request->arg0, request->arg1, request->arg2, request->arg3)",
                    out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicBinaryVectorByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var binaryImmediateMethodName,
                out var binaryImmediateScalarManagedType,
                out var binaryImmediateCarrierCppType) &&
            TryMapManagedScalarCppType(binaryImmediateScalarManagedType, out var binaryImmediateScalarCppType))
        {
            return binaryImmediateMethodName switch
            {
                "AlignRight32" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [binaryImmediateCarrierCppType, binaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        binaryImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedAlignRight32<{binaryImmediateScalarCppType}, {binaryImmediateCarrierCppType}>(request->arg0, request->arg1, request->arg2)",
                    out plan),
                "AlignRight64" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-bitwise",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [binaryImmediateCarrierCppType, binaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        binaryImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedAlignRight64<{binaryImmediateScalarCppType}, {binaryImmediateCarrierCppType}>(request->arg0, request->arg1, request->arg2)",
                    out plan),
                "Shuffle2x128"
                    when string.Equals(binaryImmediateCarrierCppType, "RuntimeIntrinsicVector256Carrier", StringComparison.Ordinal) =>
                    TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-bitwise",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(
                            null,
                            [binaryImmediateCarrierCppType, binaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                            binaryImmediateCarrierCppType),
                        $"{GetHelperNamespace()}::VectorFixedShuffle2x128<{binaryImmediateCarrierCppType}>(request->arg0, request->arg1, request->arg2)",
                        out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicBinaryVectorIndexOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var binaryPermuteMethodName,
                out var binaryPermuteValueScalarManagedType,
                out var binaryPermuteIndexScalarManagedType,
                out var binaryPermuteCarrierCppType) &&
            TryMapManagedScalarCppType(binaryPermuteValueScalarManagedType, out var binaryPermuteValueScalarCppType) &&
            TryMapManagedScalarCppType(binaryPermuteIndexScalarManagedType, out var binaryPermuteIndexScalarCppType))
        {
            return binaryPermuteMethodName switch
            {
                "PermuteVar16x16" or "PermuteVar8x16" or "PermuteVar4x64" or "PermuteVar16x32" or "PermuteVar8x64" =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryPermuteCarrierCppType,
                        binaryPermuteCarrierCppType,
                        binaryPermuteCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedPermuteVar<{binaryPermuteValueScalarCppType}, {binaryPermuteIndexScalarCppType}, {binaryPermuteCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-bitwise",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicTernaryVectorIndexedBlendOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var ternaryPermuteMethodName,
                out var ternaryPermuteValueScalarManagedType,
                out var ternaryPermuteIndexScalarManagedType,
                out var ternaryPermuteCarrierCppType) &&
            TryMapManagedScalarCppType(ternaryPermuteValueScalarManagedType, out var ternaryPermuteValueScalarCppType) &&
            TryMapManagedScalarCppType(ternaryPermuteIndexScalarManagedType, out var ternaryPermuteIndexScalarCppType))
        {
            return ternaryPermuteMethodName switch
            {
                "PermuteVar16x16x2" or "PermuteVar8x16x2" or "PermuteVar2x64x2" or "PermuteVar4x32x2" or "PermuteVar4x64x2" or "PermuteVar8x32x2" or "PermuteVar16x32x2" or "PermuteVar8x64x2" or "PermuteVar32x16x2" =>
                    TryCreateStaticTernaryCustomArgsPlan(
                        descriptor,
                        backend,
                        ternaryPermuteCarrierCppType,
                        ternaryPermuteCarrierCppType,
                        ternaryPermuteCarrierCppType,
                        ternaryPermuteCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedPermuteVarX2<{ternaryPermuteValueScalarCppType}, {ternaryPermuteIndexScalarCppType}, {ternaryPermuteCarrierCppType}>(request->arg0, request->arg1, request->arg2)",
                        "vector-fixed-bitwise",
                        out plan),
                _ => false,
            };
        }

        if (!TryExtractIntrinsicVectorMethodSuffix(descriptor.SubjectIdPrefix, subjectId, out var methodSuffix))
        {
            return false;
        }

        return methodSuffix switch
        {
            var current when current.StartsWith("op_BitwiseAnd:", StringComparison.Ordinal) ||
                              current.StartsWith("And:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseAnd<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-bitwise",
                    out plan),
            var current when current.StartsWith("op_BitwiseOr:", StringComparison.Ordinal) ||
                              current.StartsWith("Or:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseOr<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-bitwise",
                    out plan),
            var current when current.StartsWith("op_ExclusiveOr:", StringComparison.Ordinal) ||
                              current.StartsWith("Xor:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseXor<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-bitwise",
                    out plan),
            var current when current.StartsWith("op_OnesComplement:", StringComparison.Ordinal) ||
                              current.StartsWith("Not:", StringComparison.Ordinal) =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedOnesComplement<{descriptor.CarrierCppType}>(request->arg0)",
                    "vector-fixed-bitwise",
                    out plan),
            _ => false,
        };
    }



    private static bool TryCreateUnaryPassthroughPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryExtractIntrinsicVectorMethodSuffix(descriptor.SubjectIdPrefix, subjectId, out var methodSuffix) ||
            !methodSuffix.StartsWith("op_UnaryPlus:", StringComparison.Ordinal))
        {
            return false;
        }

        return TryCreateStaticUnaryPlan(
            descriptor,
            backend,
            descriptor.CarrierCppType,
            "request->arg0",
            "vector-unary-passthrough",
            out plan);
    }

}
