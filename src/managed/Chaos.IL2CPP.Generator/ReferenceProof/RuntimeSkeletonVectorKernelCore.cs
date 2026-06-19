using Chaos.IL2CPP.Contracts;

using System.Collections.Concurrent;

using System.Collections.Generic;

using System.Linq;



namespace Chaos.IL2CPP.Generator;



internal static partial class RuntimeSkeletonVectorKernelCore

{

    public const string ContractId = "vector-kernel-v1";



    private enum VectorNumericsTypeFamily

    {

        Vector2,

        Vector3,

        Vector4,

    }



    private enum VectorIntrinsicsTypeFamily

    {

        Vector64,

        Vector128,

        Vector256,

        Vector512,

        Sse,

        Sse2,

        Sse41,

        Sse42,

        Ssse3,

        Avx,

        Avx2,

        Avx512F,

        Avx512BW,

        Avx512DQ,

        Avx512F_PlusVL,

        Avx512BW_PlusVL,

        AdvSimd,

        AdvSimdArm64,

        PackedSimd,

    }



    private sealed record IntrinsicsTypeInfo(

        VectorIntrinsicsTypeFamily Family,

        string ExpectedSubjectIdPrefix,

        string CarrierCppType,

        string HelperPrefix,

        int FixedVectorWidthBytes);



    private static readonly RuntimeSkeletonVectorKernelSemanticDescriptor[] Descriptors =

    [

        CreateDescriptor("vector-fixed-shift", "/System.Numerics.Vector::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-shift", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-shift", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-constant", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-constant", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-capability-query", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-capability-query", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-unary-passthrough", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-unary-passthrough", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-equality", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-equality", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-comparison", "/System.Numerics.Vector<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),

        CreateDescriptor("vector-fixed-comparison", "/System.Numerics.Vector`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector256Carrier", 0, "VectorGeneric"),



        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-shift", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector64`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector128`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector256`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-constant", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-capability-query", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-bitwise", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-unary-passthrough", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-equality", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-comparison", "/System.Runtime.Intrinsics.Vector512`1::", RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector64<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector64Carrier", 0, "Vector64"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector128<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector128Carrier", 0, "Vector128"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector256<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector256Carrier", 0, "Vector256"),

        CreateDescriptor("vector-fixed-arithmetic", "/System.Runtime.Intrinsics.Vector512<", RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic, "RuntimeIntrinsicVector512Carrier", 0, "Vector512"),

    ];



    private static readonly ConcurrentDictionary<string, RuntimeSkeletonVectorKernelSemanticDescriptor?> FallbackDescriptorCache = new(StringComparer.Ordinal);



    public static RuntimeSkeletonVectorKernelSemanticDescriptor? TryResolveBySubjectId(

        string subjectId,

        TypedIlMethodArtifact? methodArtifact = null)

    {

        // Fast path: static descriptors array

        var match = Descriptors.FirstOrDefault(

            descriptor => subjectId.Contains(descriptor.SubjectIdPrefix, StringComparison.Ordinal));



        if (match is not null)

        {

            return match;

        }



        // Fallback path: metadata-driven resolution from TypedIlMethodArtifact (with cache)

        if (methodArtifact is not null)

        {

            return FallbackDescriptorCache.GetOrAdd(

                subjectId,

                static (_, artifact) => TryResolveFromMethodArtifact(artifact),

                methodArtifact);

        }



        return null;

    }



    public static RuntimeSkeletonVectorKernelSemanticDescriptor? TryResolveBySubjectId(string subjectId)

        => TryResolveBySubjectId(subjectId, null);



    private static RuntimeSkeletonVectorKernelSemanticDescriptor? TryResolveFromMethodArtifact(

        TypedIlMethodArtifact method)

    {

        var declaringType = method.Identity.DeclaringTypeSubjectId;



        // Phase 1: Try numerics types (Vector2/3/4)

        var numericsFamily = ClassifyNumericsType(declaringType);

        if (numericsFamily is not null)

        {

            return TryResolveNumericsFromArtifact(method, numericsFamily.Value);

        }



        // Phase 2: Try intrinsics types (Vector64/128/256/512, Sse, Avx, etc.)

        var intrinsicsFamily = ClassifyIntrinsicsType(declaringType);

        if (intrinsicsFamily is not null)

        {

            return TryResolveIntrinsicsFromArtifact(method, intrinsicsFamily.Value);

        }



        return null;

    }



    private static RuntimeSkeletonVectorKernelSemanticDescriptor? TryResolveNumericsFromArtifact(

        TypedIlMethodArtifact method,

        VectorNumericsTypeFamily typeFamily)

    {

        var methodName = ExtractMethodName(method.SubjectId);

        if (methodName is null)

        {

            return null;

        }



        var operationKind = MapMethodNameToOperationKind(methodName);

        if (operationKind is null)

        {

            return null;

        }



        var prefix = BuildSubjectIdPrefix(method.Identity.DeclaringTypeSubjectId);



        var (carrierCppType, componentCount, helperPrefix) = typeFamily switch

        {

            VectorNumericsTypeFamily.Vector2 => ("RuntimeNumericsVector2Carrier", 2, "Vector2"),

            VectorNumericsTypeFamily.Vector3 => ("RuntimeNumericsVector3Carrier", 3, "Vector3"),

            VectorNumericsTypeFamily.Vector4 => ("RuntimeNumericsVector4Carrier", 4, "Vector4"),

            _ => (null, 0, null),

        };



        if (carrierCppType is null)

        {

            return null;

        }



        var semanticId = MapOperationKindToSemanticId(operationKind.Value);

        if (semanticId is null)

        {

            return null;

        }



        return new RuntimeSkeletonVectorKernelSemanticDescriptor(

            semanticId,

            prefix,

            operationKind.Value,

            RuntimeSkeletonVectorKernelBackendKind.Generic,

            carrierCppType,

            "float",

            componentCount,

            helperPrefix!);

    }



    private static RuntimeSkeletonVectorKernelSemanticDescriptor? TryResolveIntrinsicsFromArtifact(

        TypedIlMethodArtifact method,

        VectorIntrinsicsTypeFamily family)

    {

        var methodName = ExtractMethodName(method.SubjectId);

        if (methodName is null)

        {

            return null;

        }



        var operationKind = MapIntrinsicsMethodNameToOperationKind(methodName);

        if (operationKind is null)

        {

            return null;

        }



        var info = GetIntrinsicsTypeInfo(family);

        var prefix = BuildSubjectIdPrefix(method.Identity.DeclaringTypeSubjectId);

        var semanticId = MapIntrinsicsOperationKindToSemanticId(operationKind.Value);

        if (semanticId is null)

        {

            return null;

        }



        return new RuntimeSkeletonVectorKernelSemanticDescriptor(

            semanticId,

            prefix,

            operationKind.Value,

            RuntimeSkeletonVectorKernelBackendKind.Generic,

            info.CarrierCppType,

            "float",

            0,

            info.HelperPrefix);

    }



    private static VectorNumericsTypeFamily? ClassifyNumericsType(string declaringType)

    {

        var lastDot = declaringType.LastIndexOf('.');

        var lastSlash = declaringType.LastIndexOf('/');

        var startIndex = lastDot > lastSlash ? lastDot + 1 : (lastSlash >= 0 ? lastSlash + 1 : 0);

        var typeName = declaringType[startIndex..];



        return typeName switch

        {

            "Vector2" => VectorNumericsTypeFamily.Vector2,

            "Vector3" => VectorNumericsTypeFamily.Vector3,

            "Vector4" => VectorNumericsTypeFamily.Vector4,

            _ => null,

        };

    }



    private static VectorIntrinsicsTypeFamily? ClassifyIntrinsicsType(string declaringType)

    {

        var lastSlash = declaringType.LastIndexOf('/');

        var typeName = lastSlash >= 0 ? declaringType[(lastSlash + 1)..] : declaringType;



        // Order matters: check more specific patterns first

        if (typeName.Contains("Avx512BW+VL", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx512BW_PlusVL;

        if (typeName.Contains("Avx512F+VL", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx512F_PlusVL;

        if (typeName.Contains("Avx512BW", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx512BW;

        if (typeName.Contains("Avx512DQ", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx512DQ;

        if (typeName.Contains("Avx512F", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx512F;

        if (typeName.Contains("Avx2", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx2;

        if (typeName.Contains("Avx", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Avx;

        if (typeName.Contains("AdvSimd+Arm64", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.AdvSimdArm64;

        if (typeName.Contains("AdvSimd", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.AdvSimd;

        if (typeName.Contains("PackedSimd", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.PackedSimd;

        if (typeName.Contains("Sse41", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Sse41;

        if (typeName.Contains("Sse42", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Sse42;

        if (typeName.Contains("Ssse3", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Ssse3;

        if (typeName.Contains("Sse2", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Sse2;

        if (typeName.Contains("Sse", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Sse;

        if (typeName.StartsWith("System.Runtime.Intrinsics.Vector512", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Vector512;

        if (typeName.StartsWith("System.Runtime.Intrinsics.Vector256", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Vector256;

        if (typeName.StartsWith("System.Runtime.Intrinsics.Vector128", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Vector128;

        if (typeName.StartsWith("System.Runtime.Intrinsics.Vector64", StringComparison.Ordinal))

            return VectorIntrinsicsTypeFamily.Vector64;



        return null;

    }



    private static IntrinsicsTypeInfo GetIntrinsicsTypeInfo(VectorIntrinsicsTypeFamily family)

    {

        return family switch

        {

            VectorIntrinsicsTypeFamily.Vector64 => new(family, "/System.Runtime.Intrinsics.Vector64::", "RuntimeIntrinsicVector64Carrier", "Vector64", 8),

            VectorIntrinsicsTypeFamily.Vector128 => new(family, "/System.Runtime.Intrinsics.Vector128::", "RuntimeIntrinsicVector128Carrier", "Vector128", 16),

            VectorIntrinsicsTypeFamily.Vector256 => new(family, "/System.Runtime.Intrinsics.Vector256::", "RuntimeIntrinsicVector256Carrier", "Vector256", 32),

            VectorIntrinsicsTypeFamily.Vector512 => new(family, "/System.Runtime.Intrinsics.Vector512::", "RuntimeIntrinsicVector512Carrier", "Vector512", 64),

            VectorIntrinsicsTypeFamily.Sse => new(family, "/System.Runtime.Intrinsics.X86.Sse::", "RuntimeIntrinsicVector128Carrier", "Sse", 16),

            VectorIntrinsicsTypeFamily.Sse2 => new(family, "/System.Runtime.Intrinsics.X86.Sse2::", "RuntimeIntrinsicVector128Carrier", "Sse2", 16),

            VectorIntrinsicsTypeFamily.Sse41 => new(family, "/System.Runtime.Intrinsics.X86.Sse41::", "RuntimeIntrinsicVector128Carrier", "Sse41", 16),

            VectorIntrinsicsTypeFamily.Sse42 => new(family, "/System.Runtime.Intrinsics.X86.Sse42::", "RuntimeIntrinsicVector128Carrier", "Sse42", 16),

            VectorIntrinsicsTypeFamily.Ssse3 => new(family, "/System.Runtime.Intrinsics.X86.Ssse3::", "RuntimeIntrinsicVector128Carrier", "Ssse3", 16),

            VectorIntrinsicsTypeFamily.Avx => new(family, "/System.Runtime.Intrinsics.X86.Avx::", "RuntimeIntrinsicVector256Carrier", "Avx", 32),

            VectorIntrinsicsTypeFamily.Avx2 => new(family, "/System.Runtime.Intrinsics.X86.Avx2::", "RuntimeIntrinsicVector256Carrier", "Avx2", 32),

            VectorIntrinsicsTypeFamily.Avx512F => new(family, "/System.Runtime.Intrinsics.X86.Avx512F::", "RuntimeIntrinsicVector512Carrier", "Avx512F", 64),

            VectorIntrinsicsTypeFamily.Avx512BW => new(family, "/System.Runtime.Intrinsics.X86.Avx512BW::", "RuntimeIntrinsicVector512Carrier", "Avx512BW", 64),

            VectorIntrinsicsTypeFamily.Avx512DQ => new(family, "/System.Runtime.Intrinsics.X86.Avx512DQ::", "RuntimeIntrinsicVector512Carrier", "Avx512DQ", 64),

            VectorIntrinsicsTypeFamily.Avx512F_PlusVL => new(family, "/System.Runtime.Intrinsics.X86.Avx512F+VL::", string.Empty, "Avx512FVL", 32),

            VectorIntrinsicsTypeFamily.Avx512BW_PlusVL => new(family, "/System.Runtime.Intrinsics.X86.Avx512BW+VL::", string.Empty, "Avx512BWVL", 32),

            VectorIntrinsicsTypeFamily.AdvSimd => new(family, "/System.Runtime.Intrinsics.Arm.AdvSimd::", "RuntimeIntrinsicVector128Carrier", "AdvSimd", 16),

            VectorIntrinsicsTypeFamily.AdvSimdArm64 => new(family, "/System.Runtime.Intrinsics.Arm.AdvSimd+Arm64::", "RuntimeIntrinsicVector128Carrier", "AdvSimdArm64", 16),

            VectorIntrinsicsTypeFamily.PackedSimd => new(family, "/System.Runtime.Intrinsics.Wasm.PackedSimd::", "RuntimeIntrinsicVector128Carrier", "PackedSimd", 16),

            _ => throw new ArgumentOutOfRangeException(nameof(family)),

        };

    }



    private static string? ExtractMethodName(string subjectId)

    {

        var sep = subjectId.IndexOf("::", StringComparison.Ordinal);

        if (sep < 0)

        {

            return null;

        }



        var nameStart = sep + 2;

        if (nameStart >= subjectId.Length)

        {

            return null;

        }



        var returnSep = subjectId.IndexOf(':', nameStart);

        var paramSep = subjectId.IndexOf('(', nameStart);



        var end = Math.Min(

            returnSep > 0 ? returnSep : int.MaxValue,

            paramSep > 0 ? paramSep : int.MaxValue);



        return end > nameStart ? subjectId[nameStart..end] : null;

    }



    private static RuntimeSkeletonVectorKernelOperationKind? MapMethodNameToOperationKind(string methodName)

    {

        return methodName switch

        {

            "get_Zero" or "get_One" or "get_UnitX" or "get_UnitY" or "get_UnitZ" or "get_UnitW"

                => RuntimeSkeletonVectorKernelOperationKind.VectorInitializer,



            "op_Addition" or "op_Subtraction" or "op_Multiply" or "op_Division" or "op_UnaryNegation"

            or "Add" or "Subtract" or "Multiply" or "Divide" or "Negate"

            or "Abs" or "Min" or "Max" or "Clamp"

            or "SquareRoot" or "Normalize" or "Lerp" or "Reflect" or "Cross"

            or "op_Equality" or "op_Inequality" or "Equals"

                => RuntimeSkeletonVectorKernelOperationKind.VectorArithmetic,



            "Dot" => RuntimeSkeletonVectorKernelOperationKind.VectorDotProduct,



            "Distance" or "DistanceSquared" or "Length" or "LengthSquared"

                => RuntimeSkeletonVectorKernelOperationKind.VectorDistance,



            "get_Item" => RuntimeSkeletonVectorKernelOperationKind.VectorIndexer,



            "GetHashCode" => RuntimeSkeletonVectorKernelOperationKind.VectorHash,



            "CopyTo" => RuntimeSkeletonVectorKernelOperationKind.VectorCopy,



            "Transform" or "TransformNormal" => RuntimeSkeletonVectorKernelOperationKind.VectorTransform,



            _ => null,

        };

    }



    private static string BuildSubjectIdPrefix(string declaringTypeSubjectId)

    {

        var assemblySep = declaringTypeSubjectId.IndexOf('/');

        var typePart = assemblySep >= 0

            ? declaringTypeSubjectId[assemblySep..]

            : "/" + declaringTypeSubjectId;



        // Closed generic: "...TypeName<ScalarType>" → "/...TypeName<"

        var genericSep = typePart.IndexOf('<', StringComparison.Ordinal);

        if (genericSep >= 0)

        {

            return typePart[..genericSep] + "<";

        }



        // Open generic: "...TypeName`1" → "/...TypeName`1::"

        if (typePart.Contains('`'))

        {

            return typePart + "::";

        }



        // Non-generic: "...TypeName" → "/...TypeName::"

        return typePart + "::";

    }



    private static string? MapOperationKindToSemanticId(RuntimeSkeletonVectorKernelOperationKind kind)

    {

        return kind switch

        {

            RuntimeSkeletonVectorKernelOperationKind.VectorInitializer => "vector-initializer",

            RuntimeSkeletonVectorKernelOperationKind.VectorArithmetic => "vector-arithmetic",

            RuntimeSkeletonVectorKernelOperationKind.VectorDotProduct => "vector-dot-product",

            RuntimeSkeletonVectorKernelOperationKind.VectorDistance => "vector-distance",

            RuntimeSkeletonVectorKernelOperationKind.VectorIndexer => "vector-indexer",

            RuntimeSkeletonVectorKernelOperationKind.VectorHash => "vector-hash",

            RuntimeSkeletonVectorKernelOperationKind.VectorCopy => "vector-copy",

            RuntimeSkeletonVectorKernelOperationKind.VectorTransform => "vector-transform",

            _ => null,

        };

    }



    private static RuntimeSkeletonVectorKernelOperationKind? MapIntrinsicsMethodNameToOperationKind(string methodName)

    {

        return methodName switch

        {

            "get_Zero" or "get_AllBitsSet"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant,



            "get_Count" or "get_IsSupported" or "get_One"

                => RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery,



            "CreateScalar" or "CreateScalarUnsafe"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedCreate,



            "Create"

                => RuntimeSkeletonVectorKernelOperationKind.VectorGenericCreate,



            "op_Addition" or "Add" or "op_Subtraction" or "Subtract"

            or "op_Multiply" or "Multiply" or "op_Division" or "Divide"

            or "Abs" or "Negate" or "op_UnaryNegation"

            or "Min" or "Max" or "MinNative" or "MaxNative"

            or "MinNumber" or "MaxNumber"

            or "AddSaturate" or "SubtractSaturate"

            or "Ceiling" or "Floor" or "Truncate" or "Round"

            or "CompareEqual" or "CompareGreaterThan" or "CompareGreaterThanOrEqual"

            or "CompareLessThan" or "CompareLessThanOrEqual"

            or "CompareNotEqual"

            or "Sqrt" or "SquareRoot"

            or "DivideBy" or "ReciprocalSqrt" or "ReciprocalEstimate"

            or "Scale" or "FusedMultiplyAdd"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic,



            "op_Equality" or "op_Inequality"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality,



            "Equals" or "EqualsFloatingPoint"

            or "GreaterThanAny" or "GreaterThanAll"

            or "LessThanAny" or "LessThanAll"

            or "ExtractMostSignificantBits"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison,



            "Not" or "OnesComplement" or "op_OnesComplement" or "op_BitwiseNot"

            or "And" or "op_BitwiseAnd"

            or "Or" or "op_BitwiseOr"

            or "Xor" or "op_ExclusiveOr"

            or "AndNot"

            or "RotateLeftVariable" or "RotateRightVariable"

            or "ClearHighBits" or "ShiftLeftLogical128BitLane" or "ShiftRightLogical128BitLane"

            or "SumAbsoluteDifferences" or "SumAbsoluteDifferencesInBlock"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise,



            "op_LeftShift" or "ShiftLeft" or "ShiftLeftLogical"

            or "op_RightShift" or "ShiftRightArithmetic"

            or "op_UnsignedRightShift" or "ShiftRightLogical"

            or "RotateLeft" or "RotateRight"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift,



            "op_UnaryPlus"

                => RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough,



            "AsByte" or "AsSByte"

            or "AsInt16" or "AsUInt16"

            or "AsInt32" or "AsUInt32"

            or "AsInt64" or "AsUInt64"

            or "AsSingle" or "AsDouble"

            or "AsNInt" or "AsNUInt"

            or "As" or "AsVector" or "AsVector128" or "AsVector256" or "AsVector512"

            or "ToScalar" or "ToVector128" or "ToVector256" or "ToVector512"

            or "Reinterpret" or "ChangeType"

                => RuntimeSkeletonVectorKernelOperationKind.VectorReinterpret,



            "Load" or "LoadAligned" or "LoadUnsafe"

            or "LoadVector128" or "LoadVector256" or "LoadVector512"

            or "LoadAlignedVector128" or "LoadAlignedVector256" or "LoadAlignedVector512"

            or "Store" or "StoreAligned" or "StoreUnsafe"

            or "StoreAlignedVector128" or "StoreAlignedVector256" or "StoreAlignedVector512"

            or "BroadcastVector128ToVector256" or "BroadcastVector128ToVector512"

            or "BroadcastVector256ToVector512"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedMemory,



            "ExtractVector128" or "InsertVector128"

            or "ExtractVector256" or "InsertVector256"

                => RuntimeSkeletonVectorKernelOperationKind.VectorFixedCompose,



            _ => null,

        };

    }



    private static string? MapIntrinsicsOperationKindToSemanticId(RuntimeSkeletonVectorKernelOperationKind kind)

    {

        return kind switch

        {

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedCreate => "vector-fixed-create",

            RuntimeSkeletonVectorKernelOperationKind.VectorGenericCreate => "vector-generic-create",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedShift => "vector-fixed-shift",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedConstant => "vector-fixed-constant",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedBitwise => "vector-fixed-bitwise",

            RuntimeSkeletonVectorKernelOperationKind.VectorUnaryPassthrough => "vector-unary-passthrough",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedArithmetic => "vector-fixed-arithmetic",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedEquality => "vector-fixed-equality",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedComparison => "vector-fixed-comparison",

            RuntimeSkeletonVectorKernelOperationKind.VectorCapabilityQuery => "vector-capability-query",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedMemory => "vector-fixed-memory",

            RuntimeSkeletonVectorKernelOperationKind.VectorFixedCompose => "vector-fixed-compose",

            RuntimeSkeletonVectorKernelOperationKind.VectorReinterpret => "vector-reinterpret",

            _ => null,

        };

    }



    public static RuntimeSkeletonVectorKernelBackendKind SelectBackend(

        TypedIlMethodArtifact method,

        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor)

    {

        _ = method;

        return descriptor.PreferredBackend;

    }



    public static bool TryCreate(

        TypedIlMethodArtifact method,

        out RuntimeSkeletonVectorKernelEmissionPlan plan)

    {

        ArgumentNullException.ThrowIfNull(method);

        return TryCreateCore(method.SubjectId, descriptor => SelectBackend(method, descriptor), method, out plan);

    }



    public static bool TryCreate(

        string subjectId,

        out RuntimeSkeletonVectorKernelEmissionPlan plan)

    {

        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        return TryCreateCore(subjectId, static _ => RuntimeSkeletonVectorKernelBackendKind.Generic, null, out plan);

    }

}
