using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonTaskPlatformFastPathPlan(
    string ContractId,
    string CapabilityArea,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape,
    string HelperStatements);

internal static class RuntimeSkeletonTaskPlatformCore
{
    public const string FastPathContractId = "task-kernel-fast-v1";

    public static bool TryCreateFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonTaskPlatformFastPathPlan plan)
    {
        plan = null!;
        if (!method.SubjectId.Contains("/System.Threading.Tasks.Task::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        return method.SubjectId switch
        {
            "System.Private.CoreLib/System.Threading.Tasks.Task::NewId:System.Int32()" =>
                TryCreateStaticFastPath(
                    method,
                    "introspection",
                    "System.Int32",
                    "const auto result = chaos_runtime_get_abi_v0()->task_kernel_new_id();\n    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateStaticFastPath(
        TypedIlMethodArtifact method,
        string capabilityArea,
        string managedReturnType,
        string helperStatements,
        out RuntimeSkeletonTaskPlatformFastPathPlan plan)
    {
        plan = null!;
        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                managedReturnType,
                out var returnShape,
                managedType => !IsKnownValueType(managedType),
                includeIndirectFlag: true))
        {
            return false;
        }

        plan = new RuntimeSkeletonTaskPlatformFastPathPlan(
            FastPathContractId,
            capabilityArea,
            argumentShapes,
            returnShape,
            helperStatements);
        return true;
    }

    private static bool IsKnownValueType(string managedType)
    {
        return managedType is
            "System.Void" or "System.Boolean" or "System.Int32" or "System.Int64" or
            "System.IntPtr" or "System.UIntPtr" or "System.Threading.Tasks.TaskCreationOptions" or
            "System.Threading.Tasks.TaskStatus" or "System.Threading.CancellationToken" or
            "System.Runtime.CompilerServices.ConfiguredTaskAwaitable" ||
            managedType.StartsWith("System.Runtime.CompilerServices.TaskAwaiter<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Runtime.CompilerServices.ConfiguredTaskAwaitable", StringComparison.Ordinal);
    }
}
