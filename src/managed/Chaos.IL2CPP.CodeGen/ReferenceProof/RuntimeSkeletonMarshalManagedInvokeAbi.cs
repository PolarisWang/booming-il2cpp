using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonMarshalManagedInvokeAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "marshal-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalManagedInvokeAbi abi) =>
        RuntimeSkeletonMarshalManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonMarshalManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Runtime.InteropServices.Marshal::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !IsSupportedMethodRole(method.MethodRole))
        {
            return false;
        }

        if (RuntimeSkeletonMarshalPlatformCore.TryCreateFastPath(method, out _))
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        abi = new RuntimeSkeletonMarshalManagedInvokeAbi(
            RuntimeSkeletonMarshalManagedInvokeAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsSupportedMethodRole(string methodRole)
    {
        return string.Equals(methodRole, "static-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "static-forwarder", StringComparison.Ordinal) ||
               string.Equals(methodRole, "imported-method", StringComparison.Ordinal);
    }
}
