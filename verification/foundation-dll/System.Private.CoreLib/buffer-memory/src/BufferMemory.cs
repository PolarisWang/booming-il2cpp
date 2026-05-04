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

public partial class BufferMemory
{
    // ========== Benchmark 方法 ==========

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ByteLength()
    {
        _ = Buffer.ByteLength(new byte[100]);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        _ = Buffer.GetByte(arr, 2);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        Buffer.SetByte(arr, 1, 99);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_BlockCopy()
    {
        byte[] src = { 1, 2, 3, 4, 5, 6, 7, 8 };
        byte[] dst = new byte[8];
        Buffer.BlockCopy(src, 2, dst, 0, 4);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Memmove()
    {
        Span<byte> buffer = stackalloc byte[64];
        _ = Buffer.ByteLength(buffer.ToArray());
    }

    // ========== 热更新方法 (Host -> Patch) ==========

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ByteLength()
    {
        _ = Buffer.ByteLength(new byte[100]);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_GetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        _ = Buffer.GetByte(arr, 2);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_SetByte()
    {
        byte[] arr = { 10, 20, 30, 40, 50 };
        Buffer.SetByte(arr, 1, 99);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_BlockCopy()
    {
        byte[] src = { 1, 2, 3, 4, 5, 6, 7, 8 };
        byte[] dst = new byte[8];
        Buffer.BlockCopy(src, 2, dst, 0, 4);
    }
}
