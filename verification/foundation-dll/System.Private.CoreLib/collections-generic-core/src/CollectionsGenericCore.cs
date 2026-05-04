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

public partial class CollectionsGenericCore
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试方法
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ListSort_Handwrite()
    {
        var list = new List<int> { 9, 5, 7, 3, 1, 8, 6, 2, 4, 0 };
        list.Sort();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ListToArray_Handwrite()
    {
        var list = new List<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        _ = list.ToArray();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DictionaryCount_Handwrite()
    {
        var dict = new Dictionary<string, int> { { "a", 1 }, { "b", 2 }, { "c", 3 } };
        _ = dict.Count;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_HashSetAdd_Handwrite()
    {
        var set = new HashSet<int>();
        set.Add(42);
        set.Add(99);
        set.Add(123);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新宿主侧方法 (HostToPatch)
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ListAdd_HostToPatch()
    {
        var list = new List<int>();
        list.Add(100);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ListContains_HostToPatch()
    {
        var list = new List<int> { 1, 2, 3, 4, 5 };
        _ = list.Contains(3);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DictionaryContainsKey_HostToPatch()
    {
        var dict = new Dictionary<string, int> { { "key", 1 } };
        _ = dict.ContainsKey("key");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_HashSetRemove_HostToPatch()
    {
        var set = new HashSet<int> { 1, 2, 3 };
        set.Remove(2);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DictionaryRemove_HostToPatch()
    {
        var dict = new Dictionary<string, int> { { "remove_me", 99 } };
        dict.Remove("remove_me");
    }
}
