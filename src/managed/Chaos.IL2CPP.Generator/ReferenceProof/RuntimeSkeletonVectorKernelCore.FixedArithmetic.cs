using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{

    private static bool TryCreateFixedArithmeticPlan(
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
                out var unaryInputScalarManagedType,
                out var unaryReturnScalarManagedType,
                out var unaryParameterCppType,
                out var unaryReturnCarrierCppType) &&
            TryMapManagedScalarCppType(unaryInputScalarManagedType, out var unaryInputScalarCppType) &&
            TryMapManagedScalarCppType(unaryReturnScalarManagedType, out var unaryReturnScalarCppType))
        {
            return unaryMethodName switch
            {
                "Abs" => TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryParameterCppType,
                    unaryReturnCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedAbs<{unaryInputScalarCppType}, {unaryReturnScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                    "vector-fixed-arithmetic",
                    out plan),
                "Negate" => TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    unaryParameterCppType,
                    unaryReturnCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedNegate<{unaryInputScalarCppType}, {unaryReturnScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                    "vector-fixed-arithmetic",
                    out plan),
                "GetExponent"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedGetExponent<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Reciprocal14"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedReciprocal14<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "ReciprocalSqrt14"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedReciprocalSqrt14<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Ceiling"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedCeiling<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Floor"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedFloor<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Truncate"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedTruncate<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Round"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(unaryInputScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(unaryInputScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedRound<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                "Sqrt" or "SquareRoot"
                    when string.Equals(unaryInputScalarManagedType, unaryReturnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticUnaryCustomArgPlan(
                        descriptor,
                        backend,
                        unaryParameterCppType,
                        unaryReturnCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedSqrt<{unaryInputScalarCppType}, {unaryReturnCarrierCppType}>(request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan),
                _ => false,
            };
        }

        if (TryExtractIntrinsicBinaryVectorOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var binaryMethodName,
                out var leftScalarManagedType,
                out var rightScalarManagedType,
                out var returnScalarManagedType,
                out var binaryCarrierCppType) &&
            string.Equals(leftScalarManagedType, rightScalarManagedType, StringComparison.Ordinal) &&
            TryMapManagedScalarCppType(leftScalarManagedType, out var binaryInputScalarCppType) &&
            TryMapManagedScalarCppType(returnScalarManagedType, out var binaryReturnScalarCppType))
        {
            return binaryMethodName switch
            {
                "Add" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedAdd<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "Subtract" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedSubtract<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "Multiply" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMultiply<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "Divide" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedDivide<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "CompareEqual" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareEqual<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "CompareNotEqual" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareNotEqual<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "CompareGreaterThan" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareGreaterThan<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "CompareGreaterThanOrEqual" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareGreaterThanOrEqual<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "CompareLessThan" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareLessThan<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "CompareLessThanOrEqual" => TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    binaryCarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCompareLessThanOrEqual<{binaryInputScalarCppType}, {binaryReturnScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
                "Min" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMin<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "Max" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMax<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "Scale"
                    when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
                         (string.Equals(leftScalarManagedType, "System.Single", StringComparison.Ordinal) ||
                          string.Equals(leftScalarManagedType, "System.Double", StringComparison.Ordinal)) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedScale<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "AddSaturate" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedAddSaturate<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "SubtractSaturate" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedSubtractSaturate<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "MinNative" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMinNative<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "MaxNative" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMaxNative<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "MinNumber" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMinNumber<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        RuntimeSkeletonVectorShiftTraitKind.None,
                        null,
                        null,
                        null,
                        null,
                        null,
                        "request->arg1",
                        out plan),
                "MaxNumber" when string.Equals(leftScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) =>
                    TryCreateStaticBinaryCustomArgsPlan(
                        descriptor,
                        backend,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        binaryCarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMaxNumber<{binaryInputScalarCppType}, {binaryCarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
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

        if (TryExtractIntrinsicUnaryVectorByteImmediateOperationShape(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var unaryImmediateMethodName,
                out var unaryImmediateScalarManagedType,
                out var unaryImmediateCarrierCppType) &&
            TryMapManagedScalarCppType(unaryImmediateScalarManagedType, out var unaryImmediateScalarCppType) &&
            (string.Equals(unaryImmediateScalarManagedType, "System.Single", StringComparison.Ordinal) ||
             string.Equals(unaryImmediateScalarManagedType, "System.Double", StringComparison.Ordinal)))
        {
            return unaryImmediateMethodName switch
            {
                "GetMantissa" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-arithmetic",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [unaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        unaryImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedGetMantissa<{unaryImmediateScalarCppType}, {unaryImmediateCarrierCppType}>(request->arg0, request->arg1)",
                    out plan),
                "RoundScale" => TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-arithmetic",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(
                        null,
                        [unaryImmediateCarrierCppType, "CHAOS_IL2CPP_UINT8"],
                        unaryImmediateCarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedRoundScale<{unaryImmediateScalarCppType}, {unaryImmediateCarrierCppType}>(request->arg0, request->arg1)",
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
                    "vector-fixed-arithmetic",
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

        string scalarManagedType;
        string methodSuffix;
        if (descriptor.SubjectIdPrefix.EndsWith("`1::", StringComparison.Ordinal))
        {
            if (!TryExtractOpenIntrinsicMethodSuffix(descriptor.SubjectIdPrefix, subjectId, out methodSuffix))
            {
                return false;
            }

            scalarManagedType = "!0";
        }
        else if (!TryExtractClosedIntrinsicScalarManagedType(
                     descriptor.SubjectIdPrefix,
                     subjectId,
                     out scalarManagedType,
                     out methodSuffix))
        {
            return false;
        }

        var hasConcreteScalarCppType = TryMapManagedScalarCppType(scalarManagedType, out var scalarCppType);
        var managedVectorTypeName = GetFixedVectorManagedTypeName(descriptor.SubjectIdPrefix);
        string? scalarTypeSubjectId = null;
        RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind = null;
        int? scalarGenericArgumentIndex = null;
        var fixedVectorWidthBytes = 0;

        if (!hasConcreteScalarCppType)
        {
            if (!TryResolveShiftScalarSelector(
                    descriptor.SubjectIdPrefix,
                    subjectId,
                    scalarManagedType,
                    out scalarTypeSubjectId,
                    out scalarResolutionKind,
                    out scalarGenericArgumentIndex) ||
                !TryGetFixedVectorWidthBytes(descriptor.SubjectIdPrefix, out fixedVectorWidthBytes))
            {
                return false;
            }
        }

        return methodSuffix switch
        {
            var current when (current.StartsWith("op_Addition:", StringComparison.Ordinal) ||
                              current.StartsWith("Add:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedAdd<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    out plan),
            var current when (current.StartsWith("op_Subtraction:", StringComparison.Ordinal) ||
                              current.StartsWith("Subtract:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedSubtract<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    out plan),
            var current when (current.StartsWith("op_Multiply:", StringComparison.Ordinal) ||
                              current.StartsWith("Multiply:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinarySameCarrierPlan(
                        descriptor,
                        backend,
                        descriptor.CarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMultiply<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-arithmetic",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, descriptor.CarrierCppType], descriptor.CarrierCppType),
                        string.Empty,
                        "fixed-multiply",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when (current.StartsWith("op_Multiply:", StringComparison.Ordinal) ||
                              current.StartsWith("Multiply:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{scalarManagedType})", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        [descriptor.CarrierCppType, scalarCppType],
                        descriptor.CarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMultiplyScalar<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-arithmetic",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, "void*"], descriptor.CarrierCppType),
                        string.Empty,
                        "fixed-multiply-scalar-right",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when (current.StartsWith("op_Multiply:", StringComparison.Ordinal) ||
                              current.StartsWith("Multiply:", StringComparison.Ordinal)) &&
                              current.Contains($"({scalarManagedType},{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        [scalarCppType, descriptor.CarrierCppType],
                        descriptor.CarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedMultiplyScalar<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg1, request->arg0)",
                        "vector-fixed-arithmetic",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-arithmetic",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, ["void*", descriptor.CarrierCppType], descriptor.CarrierCppType),
                        string.Empty,
                        "fixed-multiply-scalar-left",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when (current.StartsWith("op_Division:", StringComparison.Ordinal) ||
                              current.StartsWith("Divide:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinarySameCarrierPlan(
                        descriptor,
                        backend,
                        descriptor.CarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedDivide<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-arithmetic",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, descriptor.CarrierCppType], descriptor.CarrierCppType),
                        string.Empty,
                        "fixed-divide",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when (current.StartsWith("op_Division:", StringComparison.Ordinal) ||
                              current.StartsWith("Divide:", StringComparison.Ordinal)) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{scalarManagedType})", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        [descriptor.CarrierCppType, scalarCppType],
                        descriptor.CarrierCppType,
                        $"{GetHelperNamespace()}::VectorFixedDivideScalar<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-arithmetic",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-arithmetic",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, "void*"], descriptor.CarrierCppType),
                        string.Empty,
                        "fixed-divide-scalar-right",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when current.StartsWith("op_BitwiseAnd:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseAnd<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    out plan),
            var current when current.StartsWith("op_BitwiseOr:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseOr<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    out plan),
            var current when current.StartsWith("op_ExclusiveOr:", StringComparison.Ordinal) =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBitwiseXor<{descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                    "vector-fixed-arithmetic",
                    out plan),
            var current when current.StartsWith("op_OnesComplement:", StringComparison.Ordinal) =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedOnesComplement<{descriptor.CarrierCppType}>(request->arg0)",
                    "vector-fixed-arithmetic",
                    out plan),
            _ => false,
        };
    }

}
