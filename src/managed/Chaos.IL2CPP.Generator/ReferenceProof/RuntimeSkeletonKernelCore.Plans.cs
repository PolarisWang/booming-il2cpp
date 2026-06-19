using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonKernelCore
{

    private static bool TryCreateInstanceUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(descriptor.ScalarCppType, [descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }



    private static bool TryCreateStaticUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(null, [descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }



    private static bool TryCreateStaticBinaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(null, [descriptor.ScalarCppType, descriptor.ScalarCppType], returnCppType),
            helperCallExpression,
            out plan);
    }



    private static bool TryCreateStaticTernaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreatePlanCore(
            descriptor,
            backend,
            new RuntimeSkeletonKernelCarrierSchema(
                null,
                [descriptor.ScalarCppType, descriptor.ScalarCppType, descriptor.ScalarCppType],
                returnCppType),
            helperCallExpression,
            out plan);
    }



    private static bool TryCreateWideBinaryInstancePlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateInstanceUnaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }



    private static bool TryCreateWideBinaryStaticPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateStaticBinaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }



    private static bool TryCreatePointerBackedStaticBinaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateStaticBinaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }



    private static bool TryCreatePointerBackedInstanceUnaryPlan(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        string returnCppType,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        return TryCreateInstanceUnaryPlan(descriptor, backend, returnCppType, helperCallExpression, out plan);
    }



    private static bool TryCreatePlanCore(
        RuntimeSkeletonKernelSemanticDescriptor descriptor,
        RuntimeSkeletonKernelBackendKind backend,
        RuntimeSkeletonKernelCarrierSchema carrierSchema,
        string helperCallExpression,
        out RuntimeSkeletonKernelEmissionPlan plan)
    {
        plan = new RuntimeSkeletonKernelEmissionPlan(
            ContractId,
            descriptor.SemanticId,
            backend,
            carrierSchema,
            helperCallExpression);
        return true;
    }



    private static string GetHelperNamespace() => "chaos::il2cpp::runtime_core";

}
