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

    private static bool TryCreateCapabilityQueryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryResolveCapabilityScalarSelector(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var methodSuffix,
                out var scalarTypeSubjectId,
                out var scalarResolutionKind,
                out var scalarGenericArgumentIndex) ||
            !TryGetFixedVectorWidthBytes(descriptor.SubjectIdPrefix, out var fixedVectorWidthBytes))
        {
            return false;
        }

        return methodSuffix switch
        {
            var current when current.StartsWith("get_Count:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], "CHAOS_IL2CPP_INT32"),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            var current when current.StartsWith("get_IsSupported:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], "bool"),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            var current when current.StartsWith("get_One:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            _ => false,
        };
    }

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
