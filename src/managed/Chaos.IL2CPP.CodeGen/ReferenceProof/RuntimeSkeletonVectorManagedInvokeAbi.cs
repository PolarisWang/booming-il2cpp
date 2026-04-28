using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonVectorManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "identity-struct-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonVectorManagedInvokeAbi abi) =>
        RuntimeSkeletonVectorManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonVectorManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonVectorManagedInvokeAbi abi)
    {
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.VectorManagedInvokeFamilyId,
                RuntimeSkeletonVectorManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor,
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType))
        {
            return false;
        }

        abi = new RuntimeSkeletonVectorManagedInvokeAbi(
            descriptor.ContractId,
            descriptor.ThisFieldDeclaration,
            descriptor.ThisValidationStatement,
            descriptor.ThisArgumentExpression,
            descriptor.ArgumentShapes,
            descriptor.ReturnShape);
        return true;
    }
}
