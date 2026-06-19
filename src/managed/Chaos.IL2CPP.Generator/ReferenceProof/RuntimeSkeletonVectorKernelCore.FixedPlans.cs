using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{
    private static bool TryCreateFixedEqualityPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
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
            var current when current.StartsWith("op_Equality:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        [descriptor.CarrierCppType, descriptor.CarrierCppType],
                        "bool",
                        $"{GetHelperNamespace()}::VectorFixedEquals<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-equality",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-equality",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, descriptor.CarrierCppType], "bool"),
                        string.Empty,
                        "fixed-equality",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            var current when current.StartsWith("op_Inequality:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        [descriptor.CarrierCppType, descriptor.CarrierCppType],
                        "bool",
                        $"!{GetHelperNamespace()}::VectorFixedEquals<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0, request->arg1)",
                        "vector-fixed-equality",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor,
                        "vector-fixed-equality",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType, descriptor.CarrierCppType], "bool"),
                        string.Empty,
                        "fixed-inequality",
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan),
            _ => false,
        };
    }

    private static bool TryCreateFixedComparisonPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
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

        var helperNs = GetHelperNamespace();
        var carrierType = descriptor.CarrierCppType;
        var twoArgs = new[] { carrierType, carrierType };

        return methodSuffix switch
        {
            // Category B: mask-returning Equals
            var current when current.StartsWith("Equals:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    twoArgs,
                    carrierType,
                    $"{helperNs}::VectorFixedCompareEqual<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1)",
                    "vector-fixed-comparison",
                    out plan),

            // Category B: mask-returning EqualsFloatingPoint
            var current when current.StartsWith("EqualsFloatingPoint:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    twoArgs,
                    carrierType,
                    $"{helperNs}::VectorFixedCompareEqualFloatingPoint<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1)",
                    "vector-fixed-comparison",
                    out plan),

            // Category A: GreaterThanAny → CompareGreaterThan + AnyLaneNonZero
            var current when current.StartsWith("GreaterThanAny:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        twoArgs,
                        "bool",
                        $"{helperNs}::VectorFixedAnyLaneNonZero<{scalarCppType}, {carrierType}>" +
                        $"({helperNs}::VectorFixedCompareGreaterThan<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1))",
                        "vector-fixed-comparison",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor, "vector-fixed-comparison", backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, twoArgs, "bool"),
                        string.Empty, "greater-than-any",
                        scalarTypeSubjectId, fixedVectorWidthBytes, scalarResolutionKind, scalarGenericArgumentIndex,
                        out plan),

            // Category A: GreaterThanAll → CompareGreaterThan + AllLanesNonZero
            var current when current.StartsWith("GreaterThanAll:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        twoArgs,
                        "bool",
                        $"{helperNs}::VectorFixedAllLanesNonZero<{scalarCppType}, {carrierType}>" +
                        $"({helperNs}::VectorFixedCompareGreaterThan<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1))",
                        "vector-fixed-comparison",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor, "vector-fixed-comparison", backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, twoArgs, "bool"),
                        string.Empty, "greater-than-all",
                        scalarTypeSubjectId, fixedVectorWidthBytes, scalarResolutionKind, scalarGenericArgumentIndex,
                        out plan),

            // Category A: LessThanAny → CompareLessThan + AnyLaneNonZero
            var current when current.StartsWith("LessThanAny:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        twoArgs,
                        "bool",
                        $"{helperNs}::VectorFixedAnyLaneNonZero<{scalarCppType}, {carrierType}>" +
                        $"({helperNs}::VectorFixedCompareLessThan<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1))",
                        "vector-fixed-comparison",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor, "vector-fixed-comparison", backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, twoArgs, "bool"),
                        string.Empty, "less-than-any",
                        scalarTypeSubjectId, fixedVectorWidthBytes, scalarResolutionKind, scalarGenericArgumentIndex,
                        out plan),

            // Category A: LessThanAll → CompareLessThan + AllLanesNonZero
            var current when current.StartsWith("LessThanAll:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>,{managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticBinaryMixedPlan(
                        descriptor,
                        backend,
                        twoArgs,
                        "bool",
                        $"{helperNs}::VectorFixedAllLanesNonZero<{scalarCppType}, {carrierType}>" +
                        $"({helperNs}::VectorFixedCompareLessThan<{scalarCppType}, {scalarCppType}, {carrierType}>(request->arg0, request->arg1))",
                        "vector-fixed-comparison",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor, "vector-fixed-comparison", backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, twoArgs, "bool"),
                        string.Empty, "less-than-all",
                        scalarTypeSubjectId, fixedVectorWidthBytes, scalarResolutionKind, scalarGenericArgumentIndex,
                        out plan),

            // Category C: ExtractMostSignificantBits → unary, returns uint32
            var current when current.StartsWith("ExtractMostSignificantBits:", StringComparison.Ordinal) &&
                              current.Contains($"({managedVectorTypeName}<{scalarManagedType}>)", StringComparison.Ordinal) =>
                hasConcreteScalarCppType
                    ? TryCreateStaticUnaryPlan(
                        descriptor,
                        backend,
                        "CHAOS_IL2CPP_UINT32",
                        $"{helperNs}::VectorFixedExtractMostSignificantBits<{scalarCppType}, {carrierType}>(request->arg0)",
                        "vector-fixed-comparison",
                        out plan)
                    : TryCreatePlanCore(
                        descriptor, "vector-fixed-comparison", backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [carrierType], "CHAOS_IL2CPP_UINT32"),
                        string.Empty, "extract-most-significant-bits",
                        scalarTypeSubjectId, fixedVectorWidthBytes, scalarResolutionKind, scalarGenericArgumentIndex,
                        out plan),

            _ => false,
        };
    }

    private static bool TryCreateStaticNullaryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, [], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticUnaryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, [descriptor.CarrierCppType], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticUnaryCustomArgPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string argumentCppType,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, [argumentCppType], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticBinaryCustomArgsPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string leftArgumentCppType,
        string rightArgumentCppType,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        RuntimeSkeletonVectorShiftTraitKind shiftTraitKind,
        string? shiftOperationId,
        string? scalarTypeSubjectId,
        int? fixedVectorWidthBytes,
        RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind,
        int? scalarGenericArgumentIndex,
        string shiftCountValueExpression,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = new RuntimeSkeletonVectorKernelEmissionPlan(
            ContractId,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, [leftArgumentCppType, rightArgumentCppType], returnCppType),
            helperCallExpression,
            null,
            null,
            null,
            null,
            false,
            null,
            scalarTypeSubjectId,
            fixedVectorWidthBytes,
            scalarResolutionKind,
            scalarGenericArgumentIndex,
            shiftTraitKind,
            shiftOperationId,
            rightArgumentCppType,
            shiftCountValueExpression);
        return true;
    }

}
