using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonTaskManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "task-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonTaskManagedInvokeAbi abi) =>
        RuntimeSkeletonTaskManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonTaskManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonTaskManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Threading.Tasks.Task::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
                supportsStaticForwarder: true,
                supportsInstanceMethods: true,
                supportsConstructors: true,
                supportsInstanceFieldGetters: true))
        {
            return false;
        }

        if (RuntimeSkeletonTaskPlatformCore.TryCreateFastPath(method, out _))
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                IsDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: true,
            supportsInstanceFieldGetters: true);
        abi = new RuntimeSkeletonTaskManagedInvokeAbi(
            RuntimeSkeletonTaskManagedInvokeAbi.HotUpdateStableContractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsDirectReferenceReturnType(string managedType)
    {
        return !IsKnownValueType(managedType);
    }

    private static bool IsKnownValueType(string managedType)
    {
        return managedType is
            "System.Void" or "System.Boolean" or "System.Byte" or "System.SByte" or "System.Char" or
            "System.Int16" or "System.UInt16" or "System.Int32" or "System.UInt32" or
            "System.Int64" or "System.UInt64" or "System.IntPtr" or "System.UIntPtr" or
            "System.Single" or "System.Double" or "System.Threading.CancellationToken" or
            "System.Threading.Tasks.TaskCreationOptions" or "System.Threading.Tasks.TaskStatus" or
            "System.Threading.Tasks.ConfigureAwaitOptions" or
            "System.Runtime.CompilerServices.ConfiguredTaskAwaitable" or
            "System.Runtime.CompilerServices.TaskAwaiter" ||
            managedType.StartsWith("System.Runtime.CompilerServices.TaskAwaiter<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Runtime.CompilerServices.ConfiguredTaskAwaitable", StringComparison.Ordinal);
    }
}
