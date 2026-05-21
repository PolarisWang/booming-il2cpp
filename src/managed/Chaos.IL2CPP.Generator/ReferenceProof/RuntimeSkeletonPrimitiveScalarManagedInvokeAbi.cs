using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonPrimitiveScalarManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "primitive-scalar-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonPrimitiveScalarManagedInvokeAbi abi) =>
        RuntimeSkeletonPrimitiveScalarManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonPrimitiveScalarManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonPrimitiveScalarManagedInvokeAbi abi)
    {
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.PrimitiveScalarFamilyId,
                RuntimeSkeletonPrimitiveScalarManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor,
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType))
        {
            return false;
        }

        abi = new RuntimeSkeletonPrimitiveScalarManagedInvokeAbi(
            descriptor.ContractId,
            descriptor.ThisFieldDeclaration,
            descriptor.ThisValidationStatement,
            descriptor.ThisArgumentExpression,
            descriptor.ArgumentShapes,
            descriptor.ReturnShape);
        return true;
    }
}
