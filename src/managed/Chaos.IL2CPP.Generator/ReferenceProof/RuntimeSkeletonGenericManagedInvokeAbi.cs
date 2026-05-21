using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonGenericManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(static shape => shape.ValidationStatement)
            .Where(static statement => !string.IsNullOrWhiteSpace(statement)));
}

internal static class RuntimeSkeletonGenericManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        string contractId,
        out RuntimeSkeletonGenericManagedInvokeAbi abi,
        Func<string, bool>? isDirectReferenceReturnType = null)
    {
        abi = null!;
        if (!string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
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
                isDirectReferenceReturnType ?? RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: true,
            supportsInstanceFieldGetters: true);
        abi = new RuntimeSkeletonGenericManagedInvokeAbi(
            contractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }
}
