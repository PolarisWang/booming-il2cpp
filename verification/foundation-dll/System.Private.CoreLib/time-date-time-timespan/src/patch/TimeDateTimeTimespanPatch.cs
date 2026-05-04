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

public static partial class TimeDateTimeTimespanPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — DateTime / TimeSpan
    // ============================================================

    /// <summary>
    /// Patch 侧验证 DateTime..ctor(y,m,d) 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_DateTime_Ctor_YMD()
    {
        _ = new DateTime(2024, 1, 1);
    }

    /// <summary>
    /// Patch 侧验证 DateTime.AddDays 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_DateTime_AddDays()
    {
        _ = DateTime.UtcNow.AddDays(42.0);
    }

    /// <summary>
    /// Patch 侧验证 TimeSpan.FromDays 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_TimeSpan_FromDays()
    {
        _ = TimeSpan.FromDays(42.0);
    }
}
