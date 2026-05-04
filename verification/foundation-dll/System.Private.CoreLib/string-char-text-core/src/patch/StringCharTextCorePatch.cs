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

public static partial class StringCharTextCorePatch
{
    // ============================================================
    // [HotUpdateDirection(PatchToHost)] 热更新补丁侧方法
    // 验证热更新后从 Patch 侧调用宿主侧 String/Char 方法的正确性
    // ============================================================

    /// <summary>
    /// 验证 Patch 侧调用 String.Concat 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_StringConcatThree_PatchToHost()
    {
        _ = string.Concat("patch", "to", "host");
    }

    /// <summary>
    /// 验证 Patch 侧调用 String.Contains 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_StringContains_PatchToHost()
    {
        _ = "patch side string".Contains("side");
    }

    /// <summary>
    /// 验证 Patch 侧调用 String.Substring(Int32,Int32) 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_StringSubstringRange_PatchToHost()
    {
        _ = "patch-to-host".Substring(0, 5);
    }

    /// <summary>
    /// 验证 Patch 侧调用 String.Format 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_StringFormat_PatchToHost()
    {
        _ = string.Format("Patch: {0}", 99);
    }

    /// <summary>
    /// 验证 Patch 侧调用 Char.IsWhiteSpace 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CharIsWhiteSpace_PatchToHost()
    {
        _ = char.IsWhiteSpace('\t');
    }
}
