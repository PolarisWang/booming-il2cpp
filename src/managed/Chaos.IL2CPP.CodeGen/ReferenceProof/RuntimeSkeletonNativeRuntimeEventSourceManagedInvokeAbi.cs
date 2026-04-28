using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "native-runtime-event-source-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi abi) =>
        RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Diagnostics.Tracing.NativeRuntimeEventSource::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
                supportsStaticForwarder: true,
                supportsInstanceMethods: true,
                supportsConstructors: true,
                supportsInstanceFieldGetters: true))
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                IsDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: true,
            supportsInstanceFieldGetters: true);
        abi = new RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi(
            RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi.HotUpdateStableContractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsDirectReferenceReturnType(string managedType)
    {
        return managedType is "System.String" or "System.Object" ||
               managedType.EndsWith("&", StringComparison.Ordinal) ||
               managedType.EndsWith("*", StringComparison.Ordinal) ||
               managedType.EndsWith("[]", StringComparison.Ordinal);
    }
}
