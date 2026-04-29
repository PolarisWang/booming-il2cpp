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

// Auto-generated benchmark skeletons for BufferMemoryBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class BufferMemoryBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Buffer.BlockCopy with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_In() { Buffer.BlockCopy(Array.Empty<byte>(), 42, Array.Empty<byte>(), 42, 42); }
    // Purpose: Benchmark native-runtime performance of Buffer.BulkMoveWithWriteBarrier with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_System_Byte_System_UIntPtr() { }
    // Purpose: Benchmark native-runtime performance of Buffer.ByteLength with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array() { _ = Buffer.ByteLength(Array.Empty<byte>()); }
    // Purpose: Benchmark native-runtime performance of Buffer.GetByte with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32() { _ = Buffer.GetByte(Array.Empty<byte>(), 42); }
    // Purpose: Benchmark native-runtime performance of Buffer.Memcpy with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Byte_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Buffer.Memcpy with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Int32_System_Byte_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Buffer.Memmove with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_System_Byte_System_UIntPtr() { }
    // Purpose: Benchmark native-runtime performance of Buffer.MemoryCopy with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_Int64_System_Int64() { }
    // Purpose: Benchmark native-runtime performance of Buffer.MemoryCopy with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_UInt64_System_UInt64() { }
    // Purpose: Benchmark native-runtime performance of Buffer.SetByte with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte() { Buffer.SetByte(Array.Empty<byte>(), 42, (byte)42); }}
