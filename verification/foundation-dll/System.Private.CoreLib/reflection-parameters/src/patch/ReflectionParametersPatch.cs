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
/// 反射参数信息 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionParametersPatch
{
    // 复现辅助类型
    class PatchHelper
    {
        public void Sample(int x, string label = "patch") { }
    }

    /// <summary>
    /// Patch 侧：验证 GetParameters 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetParameters()
    {
        MethodInfo mi = typeof(PatchHelper).GetMethod("Sample");
        ParameterInfo[] ps = mi.GetParameters();
        Assert.Equal(2, ps.Length);
    }

    /// <summary>
    /// Patch 侧：验证 ParameterInfo.Name 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ParameterName()
    {
        MethodInfo mi = typeof(PatchHelper).GetMethod("Sample");
        Assert.Equal("x", mi.GetParameters()[0].Name);
        Assert.Equal("label", mi.GetParameters()[1].Name);
    }

    /// <summary>
    /// Patch 侧：验证 HasDefaultValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_HasDefaultValue()
    {
        MethodInfo mi = typeof(PatchHelper).GetMethod("Sample");
        Assert.False(mi.GetParameters()[0].HasDefaultValue);
        Assert.True(mi.GetParameters()[1].HasDefaultValue);
    }

    /// <summary>
    /// Patch 侧：验证 ParameterType 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ParameterType()
    {
        MethodInfo mi = typeof(PatchHelper).GetMethod("Sample");
        Assert.Equal(typeof(int), mi.GetParameters()[0].ParameterType);
        Assert.Equal(typeof(string), mi.GetParameters()[1].ParameterType);
    }
}
