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

public static partial class ArrayIndexingCopyPatch
{
    // ============================================================
    // [HotUpdateDirection(PatchToHost)] 热更新补丁侧方法
    // 验证热更新后从 Patch 侧调用宿主侧 Array 方法的正确性
    // ============================================================

    /// <summary>
    /// 验证 Patch 侧调用 Array.Copy（带偏移量）正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArrayCopyWithOffset_PatchToHost()
    {
        var src = new int[] { 10, 20, 30, 40, 50 };
        var dst = new int[5];
        Array.Copy(src, 1, dst, 0, 3);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Array.Sort（带 IComparer）正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArraySortWithComparer_PatchToHost()
    {
        Array.Sort(new int[] { 3, 1, 2 }, (IComparer<int>)null!);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Array.BinarySearch（带范围）正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArrayBinarySearchRange_PatchToHost()
    {
        _ = Array.BinarySearch(new int[] { 1, 3, 5, 7, 9, 11 }, 2, 3, 7);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Array.GetLength 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArrayGetLength_PatchToHost()
    {
        _ = new int[5, 3].GetLength(1);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Array.IndexOf 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArrayIndexOf_PatchToHost()
    {
        _ = Array.IndexOf(new string[] { "x", "y", "z" }, "y");
    }
}
