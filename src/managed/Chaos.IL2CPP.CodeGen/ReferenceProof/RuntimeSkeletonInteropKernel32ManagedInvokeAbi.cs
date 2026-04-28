using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonInteropKernel32ManagedInvokeAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "interop-kernel32-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes.Select(shape => shape.ValidationStatement).Where(static s => !string.IsNullOrWhiteSpace(s)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonInteropKernel32ManagedInvokeAbi abi) =>
        RuntimeSkeletonInteropKernel32ManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonInteropKernel32ManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonInteropKernel32ManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/Interop+Kernel32::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(method.MethodRole, supportsStaticForwarder: true))
        {
            return false;
        }

        if (RuntimeSkeletonInteropKernel32PlatformCore.TryCreateFastPath(method, out _))
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                managedType => !IsKnownValueType(managedType),
                includeIndirectFlag: true))
        {
            return false;
        }

        abi = new RuntimeSkeletonInteropKernel32ManagedInvokeAbi(
            RuntimeSkeletonInteropKernel32ManagedInvokeAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsKnownValueType(string managedType)
    {
        return managedType is
            "System.Void" or "System.Boolean" or "Interop+BOOL" or "System.Byte" or "System.SByte" or
            "System.Char" or "System.Int16" or "System.UInt16" or "System.Int32" or "System.UInt32" or
            "System.Int64" or "System.UInt64" or "System.IntPtr" or "System.UIntPtr";
    }
}
