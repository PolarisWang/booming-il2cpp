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

public partial class SpanMemoryBuffers
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试方法
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SpanSlice_Handwrite()
    {
        Span<int> s = new int[] { 1, 2, 3, 4, 5, 6, 7, 8 };
        _ = s.Slice(2);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_SpanToArray_Handwrite()
    {
        Span<int> s = new int[] { 1, 2, 3, 4, 5 };
        _ = s.ToArray();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Memory`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MemoryToArray_Handwrite()
    {
        Memory<int> m = new int[] { 1, 2, 3, 4, 5 };
        _ = m.ToArray();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MemoryMarshalGetReference_Handwrite()
    {
        Span<int> s = new int[] { 42 };
        _ = MemoryMarshal.GetReference(s);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新宿主侧方法 (HostToPatch)
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_SpanIndexer_HostToPatch()
    {
        Span<int> s = new int[] { 10, 20, 30, 40, 50 };
        _ = s[2];
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_MemorySlice_HostToPatch()
    {
        Memory<int> m = new int[] { 1, 2, 3, 4, 5 };
        _ = m.Slice(2);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_MemorySpan_HostToPatch()
    {
        Memory<int> m = new int[] { 1, 2, 3, 4, 5 };
        _ = m.Span;
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ReadOnlySpanToArray_HostToPatch()
    {
        ReadOnlySpan<int> ros = new int[] { 1, 2, 3 };
        _ = ros.ToArray();
    }
}
