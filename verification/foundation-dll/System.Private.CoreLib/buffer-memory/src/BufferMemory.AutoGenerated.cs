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

// Auto-generated skeleton for Buffer.Memory Operations (family/System.Private.CoreLib/buffer/memory).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class BufferMemory
{
    // Buffer.BlockCopy
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_In()
    {
    // TODO: needs-manual — BlockCopy with 5 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_In() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_In() { }

    // Buffer.BulkMoveWithWriteBarrier
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_System_Byte_System_UIntPtr()
    {
    // TODO: needs-manual — BulkMoveWithWriteBarrier with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_System_Byte_System_UIntPtr() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_System_Byte_System_UIntPtr() { }

    // Buffer.ByteLength
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array()
    {
    var result = Buffer.ByteLength(Array.Empty<byte>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array() { _ = Buffer.ByteLength(Array.Empty<byte>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array() { _ = Buffer.ByteLength(Array.Empty<byte>()); }

    // Buffer.GetByte
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32()
    {
    // TODO: needs-manual — GetByte with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32() { }

    // Buffer.Memcpy
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Byte_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Byte_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Byte_System_Int32() { }

    // Buffer.Memcpy
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Int32_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Int32_System_Byte_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Int32_System_Byte_System_Int32_System_Int32() { }

    // Buffer.Memmove
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_System_Byte_System_UIntPtr()
    {
    // TODO: needs-manual — Memmove with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_System_Byte_System_UIntPtr() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_System_Byte_System_UIntPtr() { }

    // Buffer.MemoryCopy
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_Int64_System_Int64()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_Int64_System_Int64() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_Int64_System_Int64() { }

    // Buffer.MemoryCopy
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_UInt64_System_UInt64()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_UInt64_System_UInt64() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_UInt64_System_UInt64() { }

    // Buffer.SetByte
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte()
    {
    // TODO: needs-manual — SetByte with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte() { }
}
