using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonEnumManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "enum-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonEnumManagedInvokeAbi abi) =>
        RuntimeSkeletonEnumManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonEnumManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonEnumManagedInvokeAbi abi)
    {
        // Shared via RuntimeSkeletonManagedInvokeAbiCore. and RuntimeSkeletonBackendSelector.
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.MetadataBackedValueTypeFamilyId,
                RuntimeSkeletonEnumManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor))
        {
            return false;
        }

        abi = new RuntimeSkeletonEnumManagedInvokeAbi(
            descriptor.ContractId,
            descriptor.ThisFieldDeclaration,
            descriptor.ThisValidationStatement,
            descriptor.ThisArgumentExpression,
            descriptor.ArgumentShapes,
            descriptor.ReturnShape);
        return true;
    }
}
