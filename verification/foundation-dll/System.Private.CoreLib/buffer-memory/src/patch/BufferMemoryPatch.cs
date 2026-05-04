using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

public static partial class BufferMemoryPatch
{
    // ========== 热更新方法 (Patch -> Host) ==========

    /// <summary>
    /// 测试目的：补丁侧验证 Buffer.ByteLength 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ByteLength()
    {
        _ = Buffer.ByteLength(new byte[100]);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Buffer.GetByte 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_GetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        _ = Buffer.GetByte(arr, 2);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Buffer.SetByte 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_SetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        Buffer.SetByte(arr, 1, 99);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Buffer.BlockCopy 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_BlockCopy()
    {
        byte[] src = { 1, 2, 3, 4, 5, 6, 7, 8 };
        byte[] dst = new byte[8];
        Buffer.BlockCopy(src, 2, dst, 0, 4);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Buffer 内存操作从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ByteLength_Array()
    {
        _ = Buffer.ByteLength(new byte[64]);
    }
}
