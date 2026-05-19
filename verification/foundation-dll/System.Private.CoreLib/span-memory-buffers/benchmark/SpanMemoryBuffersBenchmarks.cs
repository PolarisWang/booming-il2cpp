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

// Benchmark stubs for family/System.Private.CoreLib/span/memory-buffers
public static class SpanMemoryBuffersBenchmarks
{
    // Benchmark Span.get_Item native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32() { _ = new Span<byte>(new byte[4])[0]; }
    // Benchmark Span.Slice native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32() { _ = new Span<byte>(new byte[4]).Slice(1).Length; }
    // Benchmark Span.Slice native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32() { _ = new Span<byte>(new byte[4]).Slice(1, 2).Length; }
    // Benchmark Span.ToArray native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32() { _ = new byte[1].AsSpan().ToArray(); }
    // Benchmark Span.CopyTo native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1() { default(Span<byte>).CopyTo(default(Span<byte>)); }
    // Benchmark Span.get_Empty native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1() { _ = Span<byte>.Empty; }
    // Benchmark ReadOnlySpan.get_Item native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32() { _ = new ReadOnlySpan<byte>(new byte[4])[0]; }
    // Benchmark ReadOnlySpan.Slice native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32() { _ = new ReadOnlySpan<byte>(new byte[4]).Slice(1).Length; }
    // Benchmark ReadOnlySpan.ToArray native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T() { _ = default(ReadOnlySpan<byte>).ToArray(); }
    // Benchmark Memory.Slice native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32() { _ = new Memory<byte>(new byte[4]).Slice(1).Length; }
    // Benchmark Memory.get_Span native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1() { _ = default(Memory<byte>).Span; }
    // Benchmark Memory.ToArray native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_ToArray_T() { _ = default(Memory<byte>).ToArray(); }
    // Benchmark MemoryMarshal.GetReference native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1() { _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)); }
    // Benchmark MemoryMarshal.GetReference native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1() { _ = MemoryMarshal.GetReference(default(Span<byte>)); }}
