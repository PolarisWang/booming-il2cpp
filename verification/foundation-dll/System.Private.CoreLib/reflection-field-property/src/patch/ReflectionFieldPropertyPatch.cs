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

/// <summary>
/// 反射字段属性 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionFieldPropertyPatch
{
    // 复现辅助类型
    class PatchHelper
    {
        public int Id = 55;
        public string Label { get; set; } = "test";
    }

    /// <summary>
    /// Patch 侧：验证 FieldInfo.GetValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_FieldInfo_GetValue()
    {
        var obj = new PatchHelper();
        FieldInfo fi = typeof(PatchHelper).GetField("Id");
        Assert.Equal(55, fi.GetValue(obj));
    }

    /// <summary>
    /// Patch 侧：验证 FieldInfo.SetValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_FieldInfo_SetValue()
    {
        var obj = new PatchHelper();
        FieldInfo fi = typeof(PatchHelper).GetField("Id");
        fi.SetValue(obj, 99);
        Assert.Equal(99, fi.GetValue(obj));
    }

    /// <summary>
    /// Patch 侧：验证 PropertyInfo.GetValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_PropertyInfo_GetValue()
    {
        var obj = new PatchHelper();
        PropertyInfo pi = typeof(PatchHelper).GetProperty("Label");
        Assert.Equal("test", pi.GetValue(obj));
    }

    /// <summary>
    /// Patch 侧：验证 PropertyInfo.SetValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_PropertyInfo_SetValue()
    {
        var obj = new PatchHelper();
        PropertyInfo pi = typeof(PatchHelper).GetProperty("Label");
        pi.SetValue(obj, "updated");
        Assert.Equal("updated", pi.GetValue(obj));
    }
}
