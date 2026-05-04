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
/// 反射绑定标志 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionBindingPatch
{
    // 复现辅助类型
    class PatchHelper
    {
        private int _secret = 777;
        public string Greet(string name) => $"Hello {name}";
    }

    /// <summary>
    /// Patch 侧：验证 GetMethod with BindingFlags 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetMethod_WithBindingFlags()
    {
        MethodInfo mi = typeof(PatchHelper).GetMethod("Greet",
            BindingFlags.Public | BindingFlags.Instance);
        Assert.NotNull(mi);
    }

    /// <summary>
    /// Patch 侧：验证 GetField with BindingFlags 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetField_WithBindingFlags()
    {
        FieldInfo fi = typeof(PatchHelper).GetField("_secret",
            BindingFlags.NonPublic | BindingFlags.Instance);
        Assert.NotNull(fi);
        Assert.Equal(777, fi.GetValue(new PatchHelper()));
    }

    /// <summary>
    /// Patch 侧：验证 MethodBase.Invoke 默认从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_MethodBase_Invoke()
    {
        var obj = new PatchHelper();
        MethodInfo mi = typeof(PatchHelper).GetMethod("Greet");
        object result = mi.Invoke(obj, new object[] { "world" });
        Assert.Equal("Hello world", result);
    }

    /// <summary>
    /// Patch 侧：验证 GetProperty with BindingFlags 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetProperty_WithBindingFlags()
    {
        PropertyInfo pi = typeof(string).GetProperty("Length",
            BindingFlags.Public | BindingFlags.Instance);
        Assert.NotNull(pi);
    }
}
