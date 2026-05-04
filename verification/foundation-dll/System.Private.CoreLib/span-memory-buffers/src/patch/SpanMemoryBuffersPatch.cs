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

public static partial class SpanMemoryBuffersPatch
{
    // ============================================================
    // [HotUpdateDirection(PatchToHost)] 热更新补丁侧方法
    // 验证热更新后从 Patch 侧调用宿主侧 Span/Memory/MemoryMarshal 方法的正确性
    // ============================================================

    /// <summary>
    /// 验证 Patch 侧调用 Span.Slice（双参数）正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_SpanSliceTwoArgs_PatchToHost()
    {
        Span<int> s = new int[] { 1, 2, 3, 4, 5 };
        _ = s.Slice(1, 3);
    }

    /// <summary>
    /// 验证 Patch 侧调用 ReadOnlySpan.Slice 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ReadOnlySpanSlice_PatchToHost()
    {
        ReadOnlySpan<int> ros = new int[] { 10, 20, 30, 40, 50 };
        _ = ros.Slice(2);
    }

    /// <summary>
    /// 验证 Patch 侧调用 ReadOnlySpan.get_Item 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ReadOnlySpanIndexer_PatchToHost()
    {
        ReadOnlySpan<int> ros = new int[] { 100, 200, 300 };
        _ = ros[1];
    }

    /// <summary>
    /// 验证 Patch 侧调用 MemoryMarshal.GetReference（ReadOnlySpan）正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_MemoryMarshalGetReferenceRO_PatchToHost()
    {
        ReadOnlySpan<int> ros = new int[] { 42 };
        _ = MemoryMarshal.GetReference(ros);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Span.CopyTo 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_SpanMemoryBuffers)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_SpanCopyTo_PatchToHost()
    {
        Span<int> src = new int[] { 1, 2, 3 };
        Span<int> dst = new int[3];
        src.CopyTo(dst);
    }
}
