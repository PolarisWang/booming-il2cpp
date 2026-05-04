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

public partial class ArrayIndexingCopy
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试方法
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ArrayCopy_Handwrite()
    {
        var src = new int[256];
        var dst = new int[256];
        Array.Copy(src, dst, 256);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ArraySort_Handwrite()
    {
        var arr = new int[] { 9, 5, 7, 3, 1, 8, 6, 2, 4, 0 };
        Array.Sort(arr);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ArrayIndexOf_Handwrite()
    {
        var arr = new int[] { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
        _ = Array.IndexOf(arr, 50);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ArrayReverse_Handwrite()
    {
        var arr = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        Array.Reverse(arr);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新宿主侧方法 (HostToPatch)
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ArrayBinarySearch_HostToPatch()
    {
        _ = Array.BinarySearch(new int[] { 1, 3, 5, 7, 9 }, 5);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ArrayClear_HostToPatch()
    {
        var arr = new int[] { 1, 2, 3, 4, 5 };
        Array.Clear(arr, 0, 5);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ArrayGetValue_HostToPatch()
    {
        _ = new int[] { 42, 99 }.GetValue(0);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ArrayLastIndexOf_HostToPatch()
    {
        _ = Array.LastIndexOf(new int[] { 1, 2, 3, 2, 1 }, 2);
    }
}
