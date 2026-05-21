using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonFloatingScalarManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "floating-scalar-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonFloatingScalarManagedInvokeAbi abi) =>
        RuntimeSkeletonFloatingScalarManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonFloatingScalarManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonFloatingScalarManagedInvokeAbi abi)
    {
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.FloatingScalarFamilyId,
                RuntimeSkeletonFloatingScalarManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor))
        {
            return false;
        }

        abi = new RuntimeSkeletonFloatingScalarManagedInvokeAbi(
            descriptor.ContractId,
            descriptor.ThisFieldDeclaration,
            descriptor.ThisValidationStatement,
            descriptor.ThisArgumentExpression,
            descriptor.ArgumentShapes,
            descriptor.ReturnShape);
        return true;
    }
}
