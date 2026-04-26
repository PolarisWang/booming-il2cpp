using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonCharManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "char-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCharManagedInvokeAbi abi) =>
        RuntimeSkeletonCharManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonCharManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCharManagedInvokeAbi abi)
    {
        // Shared via RuntimeSkeletonManagedInvokeAbiCore. and RuntimeSkeletonBackendSelector.
        abi = null!;
        if (!RuntimeSkeletonValueTypeManagedInvokeCore.TryCreate(
                method,
                RuntimeSkeletonSemanticRegistry.PrimitiveScalarFamilyId,
                RuntimeSkeletonCharManagedInvokeAbi.HotUpdateStableContractId,
                out var descriptor,
                IsDirectReferenceReturnType))
        {
            return false;
        }

        abi = new RuntimeSkeletonCharManagedInvokeAbi(
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
