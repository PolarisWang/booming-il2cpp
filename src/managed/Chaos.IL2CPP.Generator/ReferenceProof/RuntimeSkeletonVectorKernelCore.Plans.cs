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

    private static bool TryCreateDotProductPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            var current when current == $"Dot:System.Single({GetManagedVectorTypeName(descriptor)},{GetManagedVectorTypeName(descriptor)})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    "float",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Dot(request->arg0, request->arg1)",
                    "vector-dot-product",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateDistancePlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        var vectorType = GetManagedVectorTypeName(descriptor);
        return suffix switch
        {
            var current when current == $"Distance:System.Single({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    "float",
                    $"CHAOS_IL2CPP_SQRT({GetHelperNamespace()}::{descriptor.HelperPrefix}DistanceSquared(request->arg0, request->arg1))",
                    "vector-distance",
                    out plan),
            var current when current == $"DistanceSquared:System.Single({vectorType},{vectorType})" =>
                TryCreateStaticBinarySameCarrierPlan(
                    descriptor,
                    backend,
                    "float",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}DistanceSquared(request->arg0, request->arg1)",
                    "vector-distance",
                    out plan),
            var current when current == "LengthSquared:System.Single()" =>
                TryCreateInstanceNullaryPlan(
                    descriptor,
                    backend,
                    "float",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}LengthSquared(request->this_arg)",
                    "vector-distance",
                    out plan),
            var current when current == "Length:System.Single()" =>
                TryCreateInstanceNullaryPlan(
                    descriptor,
                    backend,
                    "float",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Length(request->this_arg)",
                    "vector-distance",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateTransformPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        var vectorType = GetManagedVectorTypeName(descriptor);

        return suffix switch
        {
            var current when current == $"Transform:{vectorType}({vectorType},System.Numerics.Matrix3x2)" && descriptor.ComponentCount == 2 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix3x2Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}({vectorType},System.Numerics.Matrix3x2+Impl&)" && descriptor.ComponentCount == 2 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix3x2Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}({vectorType},System.Numerics.Matrix4x4)" && descriptor.ComponentCount is 2 or 3 or 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix4x4Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}({vectorType},System.Numerics.Matrix4x4+Impl&)" && descriptor.ComponentCount is 2 or 3 or 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix4x4Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}({vectorType},System.Numerics.Quaternion)" && descriptor.ComponentCount is 2 or 3 or 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsQuaternionCarrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"TransformNormal:{vectorType}({vectorType},System.Numerics.Matrix3x2)" && descriptor.ComponentCount == 2 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix3x2Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}TransformNormal(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"TransformNormal:{vectorType}({vectorType},System.Numerics.Matrix3x2+Impl&)" && descriptor.ComponentCount == 2 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix3x2Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}TransformNormal(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"TransformNormal:{vectorType}({vectorType},System.Numerics.Matrix4x4)" && descriptor.ComponentCount is 2 or 3 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix4x4Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}TransformNormal(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"TransformNormal:{vectorType}({vectorType},System.Numerics.Matrix4x4+Impl&)" && descriptor.ComponentCount is 2 or 3 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    descriptor.CarrierCppType,
                    "RuntimeNumericsMatrix4x4Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}TransformNormal(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector2,System.Numerics.Matrix4x4)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector2Carrier",
                    "RuntimeNumericsMatrix4x4Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector2,System.Numerics.Matrix4x4+Impl&)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector2Carrier",
                    "RuntimeNumericsMatrix4x4Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector2,System.Numerics.Quaternion)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector2Carrier",
                    "RuntimeNumericsQuaternionCarrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector3,System.Numerics.Matrix4x4)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector3Carrier",
                    "RuntimeNumericsMatrix4x4Carrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector3,System.Numerics.Matrix4x4+Impl&)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector3Carrier",
                    "RuntimeNumericsMatrix4x4Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector3,System.Numerics.Quaternion)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector3Carrier",
                    "RuntimeNumericsQuaternionCarrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector4,System.Numerics.Matrix4x4+Impl&)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector4Carrier",
                    "RuntimeNumericsMatrix4x4Carrier*",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, *request->arg1)",
                    "vector-transform",
                    RuntimeSkeletonVectorShiftTraitKind.None,
                    null,
                    null,
                    null,
                    null,
                    null,
                    "request->arg1",
                    out plan),
            var current when current == $"Transform:{vectorType}(System.Numerics.Vector4,System.Numerics.Quaternion)" && descriptor.ComponentCount == 4 =>
                TryCreateStaticBinaryCustomArgsPlan(
                    descriptor,
                    backend,
                    "RuntimeNumericsVector4Carrier",
                    "RuntimeNumericsQuaternionCarrier",
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Transform(request->arg0, request->arg1)",
                    "vector-transform",
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

    private static bool TryCreateIndexerPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            var current when current == "get_Item:System.Single(System.Int32)" =>
                TryCreateInstanceUnaryPlan(
                    descriptor,
                    backend,
                    ["CHAOS_IL2CPP_INT32"],
                    "float",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}GetElement(request->this_arg, request->arg0)",
                    "vector-indexer",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateHashPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            var current when current == "GetHashCode:System.Int32()" =>
                TryCreateInstanceNullaryPlan(
                    descriptor,
                    backend,
                    "CHAOS_IL2CPP_INT32",
                    $"{GetHelperNamespace()}::{descriptor.HelperPrefix}GetHashCode(request->this_arg)",
                    "vector-hash",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateCopyPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        return suffix switch
        {
            var current when current == "CopyTo:System.Void(System.Single[])" =>
                TryCreateCopyPlanCore(
                    descriptor,
                    backend,
                    "RuntimeSkeletonConvertSpanCarrier",
                    "reinterpret_cast<float*>(request->arg0.data)",
                    "request->arg0.length",
                    "0u",
                    false,
                    out plan),
            var current when current == "CopyTo:System.Void(System.Single[],System.Int32)" =>
                TryCreateCopyPlanCore(
                    descriptor,
                    backend,
                    "RuntimeSkeletonConvertSpanCarrier",
                    "reinterpret_cast<float*>(request->arg0.data)",
                    "request->arg0.length",
                    "static_cast<CHAOS_IL2CPP_SIZE>(request->arg1)",
                    false,
                    out plan),
            var current when current == "CopyTo:System.Void(System.Span<System.Single>)" =>
                TryCreateCopyPlanCore(
                    descriptor,
                    backend,
                    "RuntimeSkeletonConvertSpanCarrier",
                    "reinterpret_cast<float*>(request->arg0.data)",
                    "request->arg0.length",
                    "0u",
                    false,
                    out plan),
            var current when current == "TryCopyTo:System.Boolean(System.Span<System.Single>)" =>
                TryCreateCopyPlanCore(
                    descriptor,
                    backend,
                    "RuntimeSkeletonConvertSpanCarrier",
                    "reinterpret_cast<float*>(request->arg0.data)",
                    "request->arg0.length",
                    "0u",
                    true,
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateReinterpretPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string suffix,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!suffix.StartsWith("As", StringComparison.Ordinal))
        {
            return false;
        }

        return TryCreateStaticUnaryPlan(
            descriptor,
            backend,
            descriptor.CarrierCppType,
            $"{GetHelperNamespace()}::{descriptor.HelperPrefix}Reinterpret(request->arg0)",
            "vector-reinterpret",
            out plan);
    }

    private static bool TryCreateFixedCreatePlan(
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
                out _,
                out var returnScalarManagedType,
                out var parameterSignature))
        {
            return false;
        }

        var hasConcreteScalarCppType = TryMapManagedScalarCppType(returnScalarManagedType, out var scalarCppType);
        var methodBaseName = GetMethodBaseName(methodName);
        if (methodBaseName is "CreateScalar" or "CreateScalarUnsafe" &&
            hasConcreteScalarCppType &&
            string.Equals(parameterSignature, returnScalarManagedType, StringComparison.Ordinal))
        {
            return TryCreateStaticUnaryCustomArgPlan(
                descriptor,
                backend,
                scalarCppType,
                descriptor.CarrierCppType,
                $"{GetHelperNamespace()}::VectorFixedCreateScalar<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0)",
                "vector-fixed-create",
                out plan);
        }

        if (methodBaseName == "Create")
        {
            if (string.Equals(parameterSignature, returnScalarManagedType, StringComparison.Ordinal))
            {
                if (!hasConcreteScalarCppType)
                {
                    if (!TryResolveShiftScalarSelector(
                            descriptor.SubjectIdPrefix,
                            subjectId,
                            returnScalarManagedType,
                            out var scalarTypeSubjectId,
                            out var scalarResolutionKind,
                            out var scalarGenericArgumentIndex) ||
                        !TryGetFixedVectorWidthBytes(descriptor.SubjectIdPrefix, out var fixedVectorWidthBytes))
                    {
                        return false;
                    }

                    return TryCreatePlanCore(
                        descriptor,
                        "vector-generic-create",
                        backend,
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, [ "void*" ], descriptor.CarrierCppType),
                        string.Empty,
                        scalarTypeSubjectId,
                        fixedVectorWidthBytes,
                        scalarResolutionKind,
                        scalarGenericArgumentIndex,
                        out plan);
                }

                return TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    scalarCppType,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBroadcast<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0)",
                    "vector-fixed-create",
                    out plan);
            }

            var parameters = SplitTopLevelParameters(parameterSignature);
            if (parameters.Count == 0)
            {
                return false;
            }

            if (parameters.All(parameter => string.Equals(parameter, returnScalarManagedType, StringComparison.Ordinal)))
            {
                var laneArgumentTypes = Enumerable.Repeat(scalarCppType, parameters.Count).ToArray();
                var laneValueList = string.Join(
                    ", ",
                    Enumerable.Range(0, parameters.Count).Select(index => $"request->arg{index}"));
                var helperExpression =
                    $"{GetHelperNamespace()}::VectorFixedCreateFromLanes<{scalarCppType}, {descriptor.CarrierCppType}>({{ {laneValueList} }}, {parameters.Count})";
                return TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, laneArgumentTypes, descriptor.CarrierCppType),
                    helperExpression,
                    out plan);
            }

            if (parameters.Count == 2 &&
                TryResolveCarrierFromManagedVectorType(parameters[0], out var lowerScalarManagedType, out var lowerCarrierCppType, out var lowerWidthBytes) &&
                TryResolveCarrierFromManagedVectorType(parameters[1], out var upperScalarManagedType, out var upperCarrierCppType, out var upperWidthBytes) &&
                string.Equals(lowerScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
                string.Equals(upperScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
                string.Equals(lowerCarrierCppType, upperCarrierCppType, StringComparison.Ordinal) &&
                lowerWidthBytes * 2 == GetFixedVectorWidthBytesFromCarrier(descriptor.CarrierCppType))
            {
                if (!hasConcreteScalarCppType)
                {
                    return false;
                }

                return TryCreatePlanCore(
                    descriptor,
                    "vector-fixed-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [lowerCarrierCppType, upperCarrierCppType], descriptor.CarrierCppType),
                    $"{GetHelperNamespace()}::VectorFixedCreateFromHalves<{scalarCppType}, {descriptor.CarrierCppType}, {lowerCarrierCppType}>(request->arg0, request->arg1)",
                    out plan);
            }
        }

        return methodBaseName switch
        {
            "Create" =>
                TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    scalarCppType,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedBroadcast<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0)",
                    "vector-fixed-create",
                    out plan),
            "CreateScalar" or "CreateScalarUnsafe" =>
                TryCreateStaticUnaryCustomArgPlan(
                    descriptor,
                    backend,
                    scalarCppType,
                    descriptor.CarrierCppType,
                    $"{GetHelperNamespace()}::VectorFixedCreateScalar<{scalarCppType}, {descriptor.CarrierCppType}>(request->arg0)",
                    "vector-fixed-create",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateGenericCreatePlan(
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
                out _,
                out var returnScalarManagedType,
                out var parameterSignature) ||
            !TryResolveShiftScalarSelector(
                descriptor.SubjectIdPrefix,
                subjectId,
                returnScalarManagedType,
                out var scalarTypeSubjectId,
                out var scalarResolutionKind,
                out var scalarGenericArgumentIndex) ||
            !TryGetFixedVectorWidthBytes(descriptor.SubjectIdPrefix, out var fixedVectorWidthBytes))
        {
            return false;
        }

        if (!string.Equals(GetMethodBaseName(methodName), "Create", StringComparison.Ordinal))
        {
            return false;
        }

        var parameters = SplitTopLevelParameters(parameterSignature);
        if (parameters.Count == 1)
        {
            if (string.Equals(parameters[0], returnScalarManagedType, StringComparison.Ordinal))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-generic-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [ "void*" ], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan);
            }

            if (string.Equals(parameters[0], $"{returnScalarManagedType}[]", StringComparison.Ordinal))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-generic-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [ "void*" ], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan);
            }

            if (string.Equals(parameters[0], $"System.ReadOnlySpan<{returnScalarManagedType}>", StringComparison.Ordinal))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-generic-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [ "RuntimeSkeletonConvertSpanCarrier" ], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan);
            }

            return false;
        }

        if (parameters.Count == 2)
        {
            if (string.Equals(parameters[0], $"{returnScalarManagedType}[]", StringComparison.Ordinal) &&
                string.Equals(parameters[1], "System.Int32", StringComparison.Ordinal))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-generic-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [ "void*", "CHAOS_IL2CPP_INT32" ], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan);
            }

            if (TryResolveCarrierFromManagedVectorType(parameters[0], out var lowerScalarManagedType, out var lowerCarrierCppType, out var lowerWidthBytes) &&
                TryResolveCarrierFromManagedVectorType(parameters[1], out var upperScalarManagedType, out var upperCarrierCppType, out var upperWidthBytes) &&
                string.Equals(lowerScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
                string.Equals(upperScalarManagedType, returnScalarManagedType, StringComparison.Ordinal) &&
                string.Equals(lowerCarrierCppType, upperCarrierCppType, StringComparison.Ordinal) &&
                lowerWidthBytes * 2 == GetFixedVectorWidthBytesFromCarrier(descriptor.CarrierCppType))
            {
                return TryCreatePlanCore(
                    descriptor,
                    "vector-generic-create",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [ lowerCarrierCppType, upperCarrierCppType ], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan);
            }
        }

        return false;
    }

}
