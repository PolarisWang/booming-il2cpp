using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonInteropKernel32FastPathPlan(
    string ContractId,
    string CapabilityArea,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape,
    string HelperStatements);

internal static class RuntimeSkeletonInteropKernel32PlatformCore
{
    public const string FastPathContractId = "interop-kernel32-fast-v1";

    public static bool TryCreateFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonInteropKernel32FastPathPlan plan)
    {
        plan = null!;
        if (!method.SubjectId.Contains("/Interop+Kernel32::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        return method.SubjectId switch
        {
            "System.Private.CoreLib/Interop+Kernel32::GetLastError:System.Int32()" =>
                TryCreateStaticFastPath(method, "env-diagnostics-loader", "System.Int32",
                    "const auto result = chaos::il2cpp::runtime_core::InteropKernel32GetLastError();\n    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::GetCurrentProcessId:System.UInt32()" =>
                TryCreateStaticFastPath(method, "process-thread-io", "System.UInt32",
                    "const auto result = static_cast<CHAOS_IL2CPP_UINT32>(chaos::il2cpp::runtime_core::InteropKernel32GetCurrentProcessId());\n    *static_cast<CHAOS_IL2CPP_UINT32*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::GetCurrentThreadId:System.Int32()" =>
                TryCreateStaticFastPath(method, "process-thread-io", "System.Int32",
                    "const auto result = chaos::il2cpp::runtime_core::InteropKernel32GetCurrentThreadId();\n    *static_cast<CHAOS_IL2CPP_INT32*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::GetCurrentProcess:System.IntPtr()" =>
                TryCreateStaticFastPath(method, "process-thread-io", "System.IntPtr",
                    "const auto result = chaos::il2cpp::runtime_core::InteropKernel32GetCurrentProcess();\n    *static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::GetCurrentThread:System.IntPtr()" =>
                TryCreateStaticFastPath(method, "process-thread-io", "System.IntPtr",
                    "const auto result = chaos::il2cpp::runtime_core::InteropKernel32GetCurrentThread();\n    *static_cast<CHAOS_IL2CPP_INTPTR*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::CloseHandle:System.Boolean(System.IntPtr)" =>
                TryCreateStaticFastPath(method, "process-thread-io", "System.Boolean",
                    $"const auto result = chaos::il2cpp::runtime_core::InteropKernel32CloseHandle({LoadScalar("System.IntPtr", "request->arg0")});\n    *static_cast<bool*>(return_value_ptr) = result;", out plan),
            "System.Private.CoreLib/Interop+Kernel32::FreeLibrary:System.Boolean(System.IntPtr)" =>
                TryCreateStaticFastPath(method, "env-diagnostics-loader", "System.Boolean",
                    $"const auto result = chaos::il2cpp::runtime_core::InteropKernel32FreeLibrary({LoadScalar("System.IntPtr", "request->arg0")});\n    *static_cast<bool*>(return_value_ptr) = result;", out plan),
            _ => false,
        };
    }

    private static bool TryCreateStaticFastPath(
        TypedIlMethodArtifact method,
        string capabilityArea,
        string managedReturnType,
        string helperStatements,
        out RuntimeSkeletonInteropKernel32FastPathPlan plan)
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

        plan = new RuntimeSkeletonInteropKernel32FastPathPlan(
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
            "System.Void" or "System.Boolean" or "Interop+BOOL" or "System.Byte" or "System.SByte" or
            "System.Char" or "System.Int16" or "System.UInt16" or "System.Int32" or "System.UInt32" or
            "System.Int64" or "System.UInt64" or "System.IntPtr" or "System.UIntPtr";
    }

    private static string LoadScalar(string managedType, string storageExpression)
    {
        return managedType switch
        {
            "System.IntPtr" => $"*static_cast<const CHAOS_IL2CPP_INTPTR*>({storageExpression})",
            "System.UIntPtr" => $"*static_cast<const CHAOS_IL2CPP_UINTPTR*>({storageExpression})",
            "System.Int32" => $"*static_cast<const CHAOS_IL2CPP_INT32*>({storageExpression})",
            "System.UInt32" => $"*static_cast<const CHAOS_IL2CPP_UINT32*>({storageExpression})",
            "System.Boolean" => $"*static_cast<const bool*>({storageExpression})",
            _ => throw new InvalidOperationException($"unsupported interop-kernel32 scalar type '{managedType}'"),
        };
    }
}
