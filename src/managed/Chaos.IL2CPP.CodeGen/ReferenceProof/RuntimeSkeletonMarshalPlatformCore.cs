using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal enum MarshalStructureMarshallingClass
{
    FullyBlittableFastPath,
    LayoutFixedMarshallerPath,
    DescriptorDrivenMarshallerPath,
}

internal sealed record RuntimeSkeletonMarshalPlatformFastPathPlan(
    string ContractId,
    string CapabilityArea,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape,
    string HelperStatements);

internal static class RuntimeSkeletonMarshalPlatformCore
{
    public const string FastPathContractId = "marshal-platform-fast-v1";

    public static bool TryCreateFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        if (!method.SubjectId.Contains("/System.Runtime.InteropServices.Marshal::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        return TryCreateMemoryBlockFastPath(method, out plan) ||
               TryCreateStringFastPath(method, out plan) ||
               TryCreateRawReadWriteFastPath(method, out plan) ||
               TryCreateCopyFastPath(method, out plan);
    }

    public static MarshalStructureMarshallingClass ClassifyStructureMarshalling(string managedType)
    {
        if (IsFullyBlittableManagedType(managedType))
        {
            return MarshalStructureMarshallingClass.FullyBlittableFastPath;
        }

        if (IsLayoutFixedManagedType(managedType))
        {
            return MarshalStructureMarshallingClass.LayoutFixedMarshallerPath;
        }

        return MarshalStructureMarshallingClass.DescriptorDrivenMarshallerPath;
    }

    private static bool TryCreateMemoryBlockFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        return method.SubjectId switch
        {
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalAllocHGlobal(runtime, {LoadScalar("System.Int32", "request->arg0")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalAllocHGlobal(runtime, {LoadScalar("System.IntPtr", "request->arg0")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalAllocCoTaskMem(runtime, {LoadScalar("System.Int32", "request->arg0")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalReAllocHGlobal(runtime, {LoadScalar("System.IntPtr", "request->arg0")}, {LoadScalar("System.IntPtr", "request->arg1")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalReAllocCoTaskMem(runtime, {LoadScalar("System.IntPtr", "request->arg0")}, {LoadScalar("System.Int32", "request->arg1")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.Void",
                    $"(void)chaos::il2cpp::runtime_core::MarshalFreeHGlobal(runtime, {LoadScalar("System.IntPtr", "request->arg0")});",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.Void",
                    $"(void)chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(runtime, {LoadScalar("System.IntPtr", "request->arg0")});",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "memory-block",
                    "System.Void",
                    $"(void)chaos::il2cpp::runtime_core::MarshalZeroFreeCoTaskMemUtf8(runtime, {LoadScalar("System.IntPtr", "request->arg0")});",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateStringFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        return method.SubjectId switch
        {
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)" =>
                TryCreateStaticFastPath(
                    method,
                    "string-marshaling",
                    "System.IntPtr",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(runtime, thread, {LoadReference("request->arg0")});\n    {AssignReturn("System.IntPtr", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr)" =>
                TryCreateStaticFastPath(
                    method,
                    "string-marshaling",
                    "System.String",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalPtrToStringUtf8(runtime, thread, {LoadScalar("System.IntPtr", "request->arg0")}, 0, false);\n    {AssignReturn("System.String", "result")}",
                    out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)" =>
                TryCreateStaticFastPath(
                    method,
                    "string-marshaling",
                    "System.String",
                    $"const auto result = chaos::il2cpp::runtime_core::MarshalPtrToStringUtf8(runtime, thread, {LoadScalar("System.IntPtr", "request->arg0")}, {LoadScalar("System.Int32", "request->arg1")}, true);\n    {AssignReturn("System.String", "result")}",
                    out plan),
            _ => false,
        };
    }

    private static bool TryCreateRawReadWriteFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        return method.SubjectId switch
        {
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_UINT8", "System.Byte", "MarshalReadByte", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_UINT8", "System.Byte", "MarshalReadByte", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT16", "System.Int16", "MarshalReadInt16", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT16", "System.Int16", "MarshalReadInt16", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT32", "System.Int32", "MarshalReadInt32", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT32", "System.Int32", "MarshalReadInt32", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT64", "System.Int64", "MarshalReadInt64", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INT64", "System.Int64", "MarshalReadInt64", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INTPTR", "System.IntPtr", "MarshalReadIntPtr", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)" =>
                TryCreateReadFastPath(method, "CHAOS_IL2CPP_INTPTR", "System.IntPtr", "MarshalReadIntPtr", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)" =>
                TryCreateWriteFastPath(method, "MarshalWriteByte", LoadScalar("System.Byte", "request->arg1"), "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)" =>
                TryCreateWriteFastPath(method, "MarshalWriteByte", LoadScalar("System.Byte", "request->arg2"), LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt16", LoadScalar("System.Int16", "request->arg1"), "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt16", $"static_cast<CHAOS_IL2CPP_INT16>({LoadScalar("System.Char", "request->arg1")})", "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt16", LoadScalar("System.Int16", "request->arg2"), LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt16", $"static_cast<CHAOS_IL2CPP_INT16>({LoadScalar("System.Char", "request->arg2")})", LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt32", LoadScalar("System.Int32", "request->arg1"), "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt32", LoadScalar("System.Int32", "request->arg2"), LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt64", LoadScalar("System.Int64", "request->arg1"), "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)" =>
                TryCreateWriteFastPath(method, "MarshalWriteInt64", LoadScalar("System.Int64", "request->arg2"), LoadScalar("System.Int32", "request->arg1"), out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)" =>
                TryCreateWriteFastPath(method, "MarshalWriteIntPtr", LoadScalar("System.IntPtr", "request->arg1"), "0", out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)" =>
                TryCreateWriteFastPath(method, "MarshalWriteIntPtr", LoadScalar("System.IntPtr", "request->arg2"), LoadScalar("System.Int32", "request->arg1"), out plan),
            _ => false,
        };
    }

    private static bool TryCreateCopyFastPath(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        return method.SubjectId switch
        {
            // Array→Ptr (5 overloads): T[], int, IntPtr, int
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte[],System.Int32,System.IntPtr,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_UINT8", isArrayToPtr: true, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16[],System.Int32,System.IntPtr,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT16", isArrayToPtr: true, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32[],System.Int32,System.IntPtr,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT32", isArrayToPtr: true, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64[],System.Int32,System.IntPtr,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT64", isArrayToPtr: true, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr[],System.Int32,System.IntPtr,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INTPTR", isArrayToPtr: true, out plan),

            // Ptr→Array (5 overloads): IntPtr, T[], int, int
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte[],System.Int32,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_UINT8", isArrayToPtr: false, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16[],System.Int32,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT16", isArrayToPtr: false, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32[],System.Int32,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT32", isArrayToPtr: false, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64[],System.Int32,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INT64", isArrayToPtr: false, out plan),
            "System.Private.CoreLib/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr[],System.Int32,System.Int32)" =>
                TryCreateCopyFastPathCore(method, "CHAOS_IL2CPP_INTPTR", isArrayToPtr: false, out plan),

            _ => false,
        };
    }

    private static bool TryCreateCopyFastPathCore(
        TypedIlMethodArtifact method,
        string cppElementType,
        bool isArrayToPtr,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        string helperStatements;
        if (isArrayToPtr)
        {
            helperStatements =
                $"auto* chaos_array = {LoadReference("request->arg0")};\n    " +
                $"const auto chaos_start_index = {LoadScalar("System.Int32", "request->arg1")};\n    " +
                $"const auto chaos_dest = {LoadScalar("System.IntPtr", "request->arg2")};\n    " +
                $"const auto chaos_length = {LoadScalar("System.Int32", "request->arg3")};\n    " +
                $"chaos::il2cpp::runtime_core::MarshalCopyArrayToPtr<{cppElementType}>(chaos_array, chaos_start_index, chaos_dest, chaos_length);";
        }
        else
        {
            helperStatements =
                $"const auto chaos_source = {LoadScalar("System.IntPtr", "request->arg0")};\n    " +
                $"auto* chaos_array = {LoadReference("request->arg1")};\n    " +
                $"const auto chaos_start_index = {LoadScalar("System.Int32", "request->arg2")};\n    " +
                $"const auto chaos_length = {LoadScalar("System.Int32", "request->arg3")};\n    " +
                $"chaos::il2cpp::runtime_core::MarshalCopyPtrToArray<{cppElementType}>(chaos_source, chaos_array, chaos_start_index, chaos_length);";
        }

        return TryCreateStaticFastPath(
            method,
            "memory-block",
            "System.Void",
            helperStatements,
            out plan);
    }

    private static bool TryCreateReadFastPath(
        TypedIlMethodArtifact method,
        string cppType,
        string managedType,
        string helperName,
        string offsetExpression,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        return TryCreateStaticFastPath(
            method,
            "memory-block",
            managedType,
            $"const auto result = chaos::il2cpp::runtime_core::{helperName}({LoadScalar("System.IntPtr", "request->arg0")}, {offsetExpression});\n    {AssignReturn(managedType, "result", cppType)}",
            out plan);
    }

    private static bool TryCreateWriteFastPath(
        TypedIlMethodArtifact method,
        string helperName,
        string valueExpression,
        string offsetExpression,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        return TryCreateStaticFastPath(
            method,
            "memory-block",
            "System.Void",
            $"chaos::il2cpp::runtime_core::{helperName}({LoadScalar("System.IntPtr", "request->arg0")}, {offsetExpression}, {valueExpression});",
            out plan);
    }

    private static bool TryCreateStaticFastPath(
        TypedIlMethodArtifact method,
        string capabilityArea,
        string managedReturnType,
        string helperStatements,
        out RuntimeSkeletonMarshalPlatformFastPathPlan plan)
    {
        plan = null!;
        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                managedReturnType,
                out var returnShape,
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        plan = new RuntimeSkeletonMarshalPlatformFastPathPlan(
            FastPathContractId,
            capabilityArea,
            argumentShapes,
            returnShape,
            helperStatements);
        return true;
    }

    private static bool IsFullyBlittableManagedType(string managedType)
    {
        return managedType is
            "System.Byte" or "System.SByte" or "System.Int16" or "System.UInt16" or
            "System.Int32" or "System.UInt32" or "System.Int64" or "System.UInt64" or
            "System.IntPtr" or "System.UIntPtr" or "System.Single" or "System.Double";
    }

    private static bool IsLayoutFixedManagedType(string managedType)
    {
        return IsFullyBlittableManagedType(managedType) ||
               managedType is "System.Boolean" or "System.Char" or "System.Guid" or
               "System.DateTime" or "System.TimeSpan" or "System.Decimal";
    }

    private static string LoadScalar(string managedType, string storageExpression)
    {
        return managedType switch
        {
            "System.Byte" => $"*static_cast<const CHAOS_IL2CPP_UINT8*>({storageExpression})",
            "System.SByte" => $"*static_cast<const CHAOS_IL2CPP_INT8*>({storageExpression})",
            "System.Int16" => $"*static_cast<const CHAOS_IL2CPP_INT16*>({storageExpression})",
            "System.UInt16" => $"*static_cast<const CHAOS_IL2CPP_UINT16*>({storageExpression})",
            "System.Char" => $"*static_cast<const CHAOS_IL2CPP_UINT16*>({storageExpression})",
            "System.Int32" => $"*static_cast<const CHAOS_IL2CPP_INT32*>({storageExpression})",
            "System.UInt32" => $"*static_cast<const CHAOS_IL2CPP_UINT32*>({storageExpression})",
            "System.Int64" => $"*static_cast<const CHAOS_IL2CPP_INT64*>({storageExpression})",
            "System.UInt64" => $"*static_cast<const CHAOS_IL2CPP_UINT64*>({storageExpression})",
            "System.IntPtr" => $"*static_cast<const CHAOS_IL2CPP_INTPTR*>({storageExpression})",
            "System.UIntPtr" => $"*static_cast<const CHAOS_IL2CPP_UINTPTR*>({storageExpression})",
            "System.Boolean" => $"*static_cast<const bool*>({storageExpression})",
            _ => throw new InvalidOperationException($"unsupported marshal fast-path scalar type '{managedType}'"),
        };
    }

    private static string LoadReference(string storageExpression)
    {
        return $"*static_cast<void* const*>({storageExpression})";
    }

    private static string AssignReturn(string managedType, string valueExpression, string? cppTypeOverride = null)
    {
        if (RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType(managedType))
        {
            return $"*request->return_value = {valueExpression};";
        }

        var cppType = cppTypeOverride ?? managedType switch
        {
            "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
            "System.Byte" => "CHAOS_IL2CPP_UINT8",
            "System.Int16" => "CHAOS_IL2CPP_INT16",
            "System.Int32" => "CHAOS_IL2CPP_INT32",
            "System.Int64" => "CHAOS_IL2CPP_INT64",
            _ => throw new InvalidOperationException($"unsupported marshal fast-path return type '{managedType}'"),
        };
        return $"*static_cast<{cppType}*>(return_value_ptr) = {valueExpression};";
    }
}
