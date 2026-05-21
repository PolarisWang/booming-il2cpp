using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonDecimalManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "decimal-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonDecimalManagedInvokeAbi abi) =>
        RuntimeSkeletonDecimalManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonDecimalManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonDecimalManagedInvokeAbi abi)
    {
        // Shared via RuntimeSkeletonManagedInvokeAbiCore. and RuntimeSkeletonBackendSelector.
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.DecimalClusterFamilyId,
                RuntimeSkeletonDecimalManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor,
                IsDirectReferenceReturnType))
        {
            return false;
        }

        abi = new RuntimeSkeletonDecimalManagedInvokeAbi(
            descriptor.ContractId,
            descriptor.ThisFieldDeclaration,
            descriptor.ThisValidationStatement,
            descriptor.ThisArgumentExpression,
            descriptor.ArgumentShapes,
            descriptor.ReturnShape);
        return true;
    }

    private static bool IsDirectReferenceReturnType(string managedType)
    {
        return RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType(managedType);
    }
}
