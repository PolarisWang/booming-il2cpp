using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;

namespace Chaos.IL2CPP.Generator;

internal enum RuntimeSkeletonVectorKernelBackendKind
{
    Generic,
    Intrinsic,
}

internal enum RuntimeSkeletonVectorKernelOperationKind
{
    VectorInitializer,
    VectorArithmetic,
    VectorDotProduct,
    VectorDistance,
    VectorIndexer,
    VectorHash,
    VectorCopy,
    VectorTransform,
    VectorReinterpret,
    VectorFixedCreate,
    VectorFixedShift,
    VectorFixedConstant,
    VectorFixedBitwise,
    VectorUnaryPassthrough,
    VectorFixedArithmetic,
    VectorFixedEquality,
    VectorFixedComparison,
    VectorCapabilityQuery,
    VectorGenericCreate,
    VectorFixedMemory,
    VectorFixedCompose,
}

internal sealed record RuntimeSkeletonVectorKernelSemanticDescriptor(
    string SemanticId,
    string SubjectIdPrefix,
    RuntimeSkeletonVectorKernelOperationKind OperationKind,
    RuntimeSkeletonVectorKernelBackendKind PreferredBackend,
    string CarrierCppType,
    string ComponentCppType,
    int ComponentCount,
    string HelperPrefix);

internal sealed record RuntimeSkeletonVectorKernelCarrierSchema(
    string? ThisCppType,
    IReadOnlyList<string> ArgumentCppTypes,
    string ReturnCppType)
{
    public string ThisFieldDeclaration =>
        string.IsNullOrWhiteSpace(ThisCppType) ? string.Empty : $"{ThisCppType} this_arg;";

    public string ArgFieldDeclarations =>
        string.Join(
            "\n    ",
            ArgumentCppTypes.Select((cppType, index) => $"{cppType} arg{index};"));

    public string ArgValidationStatements =>
        string.Join(
            "\n    ",
            ArgumentCppTypes
                .Select(
                    static (cppType, index) => IsPointerCppType(cppType)
                        ? $"if (request->arg{index} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}"
                        : string.Empty)
                .Where(static statement => !string.IsNullOrWhiteSpace(statement)));

    public string ReturnFieldDeclaration =>
        string.IsNullOrWhiteSpace(ReturnCppType) ? string.Empty : $"{ReturnCppType}* return_value;";

    private static bool IsPointerCppType(string cppType) =>
        !string.IsNullOrWhiteSpace(cppType) && cppType.TrimEnd().EndsWith('*');
}

internal enum RuntimeSkeletonVectorKernelScalarResolutionKind
{
    ConcreteTypeToken,
    ClassGenericArgument,
    MethodGenericArgument,
}

internal enum RuntimeSkeletonVectorShiftTraitKind
{
    None,
    Signed,
    Unsigned,
    NativeSigned,
    NativeUnsigned,
    Floating,
    Deferred,
}

internal sealed record RuntimeSkeletonVectorKernelEmissionPlan(
    string ContractId,
    string SemanticId,
    RuntimeSkeletonVectorKernelBackendKind BackendKind,
    RuntimeSkeletonVectorKernelCarrierSchema CarrierSchema,
    string HelperCallExpression,
    string? DestinationArgCppType = null,
    string? DestinationDataExpression = null,
    string? DestinationLengthExpression = null,
    string? StartIndexExpression = null,
    bool ReturnsBool = false,
    string? CapabilityOperationId = null,
    string? ScalarTypeSubjectId = null,
    int? FixedVectorWidthBytes = null,
    RuntimeSkeletonVectorKernelScalarResolutionKind? ScalarResolutionKind = null,
    int? ScalarGenericArgumentIndex = null,
    RuntimeSkeletonVectorShiftTraitKind ShiftTraitKind = RuntimeSkeletonVectorShiftTraitKind.None,
    string? ShiftOperationId = null,
    string ShiftCountCppType = "CHAOS_IL2CPP_INT32",
    string ShiftCountValueExpression = "request->arg1",
    string? ScalarCppTypeExpression = null);

