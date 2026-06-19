using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{

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
                        new RuntimeSkeletonVectorKernelCarrierSchema(null, ["void*"], descriptor.CarrierCppType),
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
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, ["void*"], descriptor.CarrierCppType),
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
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, ["void*"], descriptor.CarrierCppType),
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
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, ["RuntimeSkeletonConvertSpanCarrier"], descriptor.CarrierCppType),
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
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, ["void*", "CHAOS_IL2CPP_INT32"], descriptor.CarrierCppType),
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
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [lowerCarrierCppType, upperCarrierCppType], descriptor.CarrierCppType),
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
