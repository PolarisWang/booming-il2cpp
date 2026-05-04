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

public static partial class CollectionsGenericCorePatch
{
    // ============================================================
    // [HotUpdateDirection(PatchToHost)] 热更新补丁侧方法
    // 验证热更新后从 Patch 侧调用宿主侧泛型集合方法的正确性
    // ============================================================

    /// <summary>
    /// 验证 Patch 侧调用 List.Remove 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ListRemove_PatchToHost()
    {
        var list = new List<int> { 1, 2, 3, 4, 5 };
        list.Remove(3);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Dictionary.TryGetValue 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_DictionaryTryGetValue_PatchToHost()
    {
        var dict = new Dictionary<string, int> { { "patch_key", 42 } };
        _ = dict.TryGetValue("patch_key", out int val);
    }

    /// <summary>
    /// 验证 Patch 侧调用 HashSet.Contains 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_HashSetContains_PatchToHost()
    {
        var set = new HashSet<int> { 10, 20, 30 };
        _ = set.Contains(20);
    }

    /// <summary>
    /// 验证 Patch 侧调用 List.IndexOf 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ListIndexOf_PatchToHost()
    {
        var list = new List<string> { "patch", "to", "host" };
        _ = list.IndexOf("to");
    }

    /// <summary>
    /// 验证 Patch 侧调用 Dictionary.Add 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_DictionaryAdd_PatchToHost()
    {
        var dict = new Dictionary<int, string>();
        dict.Add(1, "patch-value");
    }
}
