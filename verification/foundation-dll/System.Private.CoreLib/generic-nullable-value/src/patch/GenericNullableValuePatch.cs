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

public static partial class GenericNullableValuePatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — Nullable 操作
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Nullable<T>.get_HasValue 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Nullable_HasValue()
    {
        int? val = 42;
        _ = val.HasValue;
    }

    /// <summary>
    /// Patch 侧验证 Nullable<T>.GetValueOrDefault 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Nullable_GetValueOrDefault()
    {
        int? val = 42;
        _ = val.GetValueOrDefault();
    }

    /// <summary>
    /// Patch 侧验证 Nullable<T>.ToString 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Nullable_ToString()
    {
        int? val = 42;
        _ = val.ToString();
    }
}
