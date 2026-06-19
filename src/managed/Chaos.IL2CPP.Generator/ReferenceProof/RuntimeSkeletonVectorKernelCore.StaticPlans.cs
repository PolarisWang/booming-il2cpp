using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{
    private static bool TryCreateStaticBinarySameCarrierPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreateStaticBinaryMixedPlan(
            descriptor,
            backend,
            [descriptor.CarrierCppType, descriptor.CarrierCppType],
            returnCppType,
            helperCallExpression,
            semanticId,
            out plan);
    }

    private static bool TryCreateStaticBinaryMixedPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        IReadOnlyList<string> argumentCppTypes,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, argumentCppTypes, returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateStaticTernaryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreateStaticTernaryMixedPlan(
            descriptor,
            backend,
            [descriptor.CarrierCppType, descriptor.CarrierCppType, descriptor.CarrierCppType],
            returnCppType,
            helperCallExpression,
            semanticId,
            out plan);
    }

    private static bool TryCreateStaticTernaryCustomArgsPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string firstArgumentCppType,
        string secondArgumentCppType,
        string thirdArgumentCppType,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreateStaticTernaryMixedPlan(
            descriptor,
            backend,
            [firstArgumentCppType, secondArgumentCppType, thirdArgumentCppType],
            returnCppType,
            helperCallExpression,
            semanticId,
            out plan);
    }

    private static bool TryCreateStaticTernaryMixedPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        IReadOnlyList<string> argumentCppTypes,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(null, argumentCppTypes, returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateInstanceNullaryPlan(
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
            new RuntimeSkeletonVectorKernelCarrierSchema(descriptor.CarrierCppType, [], returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateInstanceUnaryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        IReadOnlyList<string> argumentCppTypes,
        string returnCppType,
        string helperCallExpression,
        string semanticId,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(descriptor.CarrierCppType, argumentCppTypes, returnCppType),
            helperCallExpression,
            out plan);
    }

    private static bool TryCreateCopyPlanCore(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        RuntimeSkeletonVectorKernelBackendKind backend,
        string destinationArgCppType,
        string destinationDataExpression,
        string destinationLengthExpression,
        string startIndexExpression,
        bool returnsBool,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = new RuntimeSkeletonVectorKernelEmissionPlan(
            ContractId,
            "vector-copy",
            backend,
            new RuntimeSkeletonVectorKernelCarrierSchema(
                descriptor.CarrierCppType,
                returnsBool
                    ? [destinationArgCppType]
                    : destinationArgCppType == "RuntimeSkeletonConvertSpanCarrier" && startIndexExpression != "0u"
                        ? [destinationArgCppType, "CHAOS_IL2CPP_INT32"]
                        : [destinationArgCppType],
                returnsBool ? "bool" : string.Empty),
            $"{GetHelperNamespace()}::{descriptor.HelperPrefix}TryCopyTo(",
            destinationArgCppType,
            destinationDataExpression,
            destinationLengthExpression,
            startIndexExpression,
            returnsBool);
        return true;
    }

    private static bool TryCreatePlanCore(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string semanticId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        RuntimeSkeletonVectorKernelCarrierSchema carrierSchema,
        string helperCallExpression,
        string? scalarTypeSubjectId,
        int? fixedVectorWidthBytes,
        RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind,
        int? scalarGenericArgumentIndex,
        out RuntimeSkeletonVectorKernelEmissionPlan plan) =>
        TryCreatePlanCore(
            descriptor,
            semanticId,
            backend,
            carrierSchema,
            helperCallExpression,
            null,
            scalarTypeSubjectId,
            fixedVectorWidthBytes,
            scalarResolutionKind,
            scalarGenericArgumentIndex,
            out plan);

    private static bool TryCreatePlanCore(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string semanticId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        RuntimeSkeletonVectorKernelCarrierSchema carrierSchema,
        string helperCallExpression,
        string? capabilityOperationId,
        string? scalarTypeSubjectId,
        int? fixedVectorWidthBytes,
        RuntimeSkeletonVectorKernelScalarResolutionKind? scalarResolutionKind,
        int? scalarGenericArgumentIndex,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = new RuntimeSkeletonVectorKernelEmissionPlan(
            ContractId,
            semanticId,
            backend,
            carrierSchema,
            helperCallExpression,
            null,
            null,
            null,
            null,
            false,
            capabilityOperationId,
            scalarTypeSubjectId,
            fixedVectorWidthBytes,
            scalarResolutionKind,
            scalarGenericArgumentIndex);
        return true;
    }
}
