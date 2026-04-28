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

// Auto-generated benchmark skeletons for SpanMemoryBuffersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class SpanMemoryBuffersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Span.get_Item with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_get_Item_T_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Span.Slice with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Span.Slice with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Span.ToArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Span.CopyTo with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1() { default(Span<byte>).CopyTo(default(Span<byte>)); }
    // Purpose: Benchmark native-runtime performance of Span.get_Empty with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1() { _ = Span<byte>.Empty; }
    // Purpose: Benchmark native-runtime performance of ReadOnlySpan.get_Item with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of ReadOnlySpan.Slice with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of ReadOnlySpan.ToArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T() { _ = default(ReadOnlySpan<byte>).ToArray(); }
    // Purpose: Benchmark native-runtime performance of Memory.Slice with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Memory.get_Span with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1() { _ = default(Memory<byte>).Span; }
    // Purpose: Benchmark native-runtime performance of Memory.ToArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Memory_1_ToArray_T() { _ = default(Memory<byte>).ToArray(); }
    // Purpose: Benchmark native-runtime performance of MemoryMarshal.GetReference with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_ReadOnlySpan_1() { _ = MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)); }
    // Purpose: Benchmark native-runtime performance of MemoryMarshal.GetReference with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_System_Span_1() { _ = MemoryMarshal.GetReference(default(Span<byte>)); }}
