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

public static partial class GlobalizationCulturePatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — CultureInfo
    // ============================================================

    /// <summary>
    /// Patch 侧验证 CultureInfo.get_InvariantCulture 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CultureInfo_InvariantCulture()
    {
        _ = CultureInfo.InvariantCulture;
    }

    /// <summary>
    /// Patch 侧验证 CultureInfo.get_DisplayName 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CultureInfo_DisplayName()
    {
        _ = CultureInfo.InvariantCulture.DisplayName;
    }

    /// <summary>
    /// Patch 侧验证 CompareInfo.IndexOf 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CompareInfo_IndexOf()
    {
        _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "el");
    }
}
