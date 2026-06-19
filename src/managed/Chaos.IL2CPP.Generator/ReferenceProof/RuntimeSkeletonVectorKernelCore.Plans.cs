using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{

    private static bool TryCreateCore(
        string subjectId,
        Func<RuntimeSkeletonVectorKernelSemanticDescriptor, RuntimeSkeletonVectorKernelBackendKind> backendSelector,
        TypedIlMethodArtifact? methodArtifact,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;

        // Phase 1: Fast path — static descriptors array
        var matchingDescriptors = Descriptors
            .Where(descriptor => subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal));

        foreach (var descriptor in matchingDescriptors)
        {
            var backend = backendSelector(descriptor);
            if (TryCreatePlan(descriptor, subjectId, backend, out plan))
            {
                return true;
            }
        }

        // Phase 2: Metadata-driven fallback — only when TypedIlMethodArtifact is available
        if (methodArtifact is not null)
        {
            var fallbackDescriptor = TryResolveFromMethodArtifact(methodArtifact);
            if (fallbackDescriptor is not null)
            {
                var backend = backendSelector(fallbackDescriptor);
                if (TryCreatePlan(fallbackDescriptor, subjectId, backend, out plan))
                {
                    return true;
                }
            }
        }

        return false;
    }

    private static RuntimeSkeletonVectorKernelSemanticDescriptor CreateDescriptor(
        string semanticId,
        string subjectIdPrefix,
        RuntimeSkeletonVectorKernelOperationKind operationKind,
        string carrierCppType,
        int componentCount,
        string helperPrefix)
    {
        return new RuntimeSkeletonVectorKernelSemanticDescriptor(
            semanticId,
            subjectIdPrefix,
            operationKind,
            RuntimeSkeletonVectorKernelBackendKind.Generic,
            carrierCppType,
            "float",
            componentCount,
            helperPrefix);
    }

    private static bool TryCreatePlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal))
        {
            return false;
        }

        var prefixIndex = subjectId.IndexOf(descriptor.SubjectIdPrefix, StringComparison.Ordinal);
        var suffix = subjectId[(prefixIndex + descriptor.SubjectIdPrefix.Length)..];

        return descriptor.OperationKind switch
        {
            RuntimeSkeletonVectorKernelOperationKind.VectorInitializer => TryCreateInitializerPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorArithmetic => TryCreateArithmeticPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorDotProduct => TryCreateDotProductPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorDistance => TryCreateDistancePlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorIndexer => TryCreateIndexerPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorHash => TryCreateHashPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorCopy => TryCreateCopyPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorTransform => TryCreateTransformPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedCreate => TryCreateFixedCreatePlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorGenericCreate => TryCreateGenericCreatePlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift => TryCreateFixedShiftPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorReinterpret => TryCreateReinterpretPlan(descriptor, suffix, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant => TryCreateFixedConstantPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise => TryCreateFixedBitwisePlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough => TryCreateUnaryPassthroughPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery => TryCreateCapabilityQueryPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic => TryCreateFixedArithmeticPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality => TryCreateFixedEqualityPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison => TryCreateFixedComparisonPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedMemory => TryCreateFixedMemoryPlan(descriptor, subjectId, backend, out plan),
            RuntimeSkeletonVectorKernelOperationKind.VectorFixedCompose => TryCreateFixedComposePlan(descriptor, subjectId, backend, out plan),
            _ => false,
        };
    }

    private static bool TryCreateInitializerPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            var current when current == $"get_Zero:{GetManagedVectorTypeName(descriptor)}()" =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Zero()",
                    "vector-initializer",
                    out plan),
            var current when current == $"get_One:{GetManagedVectorTypeName(descriptor)}()" =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}One()",
                    "vector-initializer",
                    out plan),
            var current when current == $"get_UnitX:{GetManagedVectorTypeName(descriptor)}()" =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}UnitX()",
                    "vector-initializer",
                    out plan),
            var current when current == $"get_UnitY:{GetManagedVectorTypeName(descriptor)}()" =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}UnitY()",
                    "vector-initializer",
                    out plan),
            var current when current == $"get_UnitZ:{GetManagedVectorTypeName(descriptor)}()" && descriptor.ComponentCount >= 3 =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}UnitZ()",
                    "vector-initializer",
                    out plan),
            var current when current == $"get_UnitW:{GetManagedVectorTypeName(descriptor)}()" && descriptor.ComponentCount >= 4 =>
                TryCreateStaticNullaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}UnitW()",
                    "vector-initializer",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateArithmeticPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        var vectorType = GetManagedVectorTypeName(descriptor);

        return suffix switch
        {
            var current when current == $"Add:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Add(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Addition:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Add(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Subtract:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Subtract(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Subtraction:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Subtract(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Equality:System.Boolean({vectorType},{vectorType})" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, descriptor.CarrierCppType],
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Inequality:System.Boolean({vectorType},{vectorType})" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, descriptor.CarrierCppType],
                    "bool",
                    $"!{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Equals:System.Boolean({vectorType})" =>
                TryCreateInstanceUnaryPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType],
                    "bool",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Equals(request->this_arg, request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Multiply:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Multiply(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Multiply:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Multiply(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Multiply:{vectorType}({vectorType},System.Single)" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, "float"],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}MultiplyScalar(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Multiply:{vectorType}({vectorType},System.Single)" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, "float"],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}MultiplyScalar(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Multiply:{vectorType}(System.Single,{vectorType})" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    ["float", descriptor.CarrierCppType],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}MultiplyScalar(request->arg1, request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Multiply:{vectorType}(System.Single,{vectorType})" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    ["float", descriptor.CarrierCppType],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}MultiplyScalar(request->arg1, request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Divide:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Divide(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Division:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Divide(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Divide:{vectorType}({vectorType},System.Single)" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, "float"],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}DivideScalar(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_Division:{vectorType}({vectorType},System.Single)" =>
                TryCreateStaticBinaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, "float"],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}DivideScalar(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Abs:{vectorType}({vectorType})" =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Abs(request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Min:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Min(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Max:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Max(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Clamp:{vectorType}({vectorType},{vectorType},{vectorType})" =>
                TryCreateStaticTernaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Clamp(request->arg0, request->arg1, request->arg2)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"SquareRoot:{vectorType}({vectorType})" =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}SquareRoot(request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Normalize:{vectorType}({vectorType})" =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Normalize(request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Lerp:{vectorType}({vectorType},{vectorType},System.Single)" =>
                TryCreateStaticTernaryMixedPlan(
                    descriptor,
                    backend,
                    [descriptor.CarrierCppType, descriptor.CarrierCppType, "float"],
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Lerp(request->arg0, request->arg1, request->arg2)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Reflect:{vectorType}({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Reflect(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Cross:{vectorType}({vectorType},{vectorType})" && descriptor.ComponentCount == 3 =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Cross(request->arg0, request->arg1)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"Negate:{vectorType}({vectorType})" =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Negate(request->arg0)",
                    "vector-arithmetic",
                    out plan),
            var current when current == $"op_UnaryNegation:{vectorType}({vectorType})" =>
                TryCreateStaticUnaryPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Negate(request->arg0)",
                    "vector-arithmetic",
                    out plan),
            _ => false,
        };
    }

}
