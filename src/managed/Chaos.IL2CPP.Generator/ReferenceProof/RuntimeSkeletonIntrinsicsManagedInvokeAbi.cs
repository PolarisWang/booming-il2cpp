using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonIntrinsicsManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "intrinsics-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(static s => !string.IsNullOrWhiteSpace(s)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonIntrinsicsManagedInvokeAbi abi) =>
        RuntimeSkeletonIntrinsicsManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonIntrinsicsManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonIntrinsicsManagedInvokeAbi abi)
    {
        abi = null!;

        if (!RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
                supportsStaticForwarder: true,
                supportsInstanceMethods: true,
                supportsConstructors: true,
                supportsInstanceFieldGetters: true) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: true,
            supportsInstanceFieldGetters: true);
        var thisFieldDeclaration = hasThisArgument ? "void* this_arg;" : string.Empty;
        var thisValidationStatement = hasThisArgument
            ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
            : string.Empty;
        var thisArgumentExpression = hasThisArgument ? "request->this_arg" : "nullptr";

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);

        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        abi = new RuntimeSkeletonIntrinsicsManagedInvokeAbi(
            RuntimeSkeletonIntrinsicsManagedInvokeAbi.HotUpdateStableContractId,
            thisFieldDeclaration,
            thisValidationStatement,
            thisArgumentExpression,
            argumentShapes,
            returnShape);
        return true;
    }
}
