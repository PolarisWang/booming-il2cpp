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
/// 反射成员完全 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionMemberCompletePatch
{
    // 复现辅助类型（Patch 侧无法直接引用 Host 内部类，需独立定义）
    class PatchHelper
    {
        public int Value = 7;
        public int Multiply(int x, int y) => x * y;
        public PatchHelper() { }
    }

    /// <summary>
    /// Patch 侧：验证 MethodInfo.Invoke 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_MethodInfo_Invoke()
    {
        var obj = new PatchHelper();
        MethodInfo mi = typeof(PatchHelper).GetMethod("Multiply");
        object result = mi.Invoke(obj, new object[] { 6, 7 });
        Assert.Equal(42, result);
    }

    /// <summary>
    /// Patch 侧：验证 ConstructorInfo.Invoke 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ConstructorInfo_Invoke()
    {
        ConstructorInfo ci = typeof(PatchHelper).GetConstructor(Type.EmptyTypes);
        object obj = ci.Invoke(null);
        Assert.NotNull(obj);
    }

    /// <summary>
    /// Patch 侧：验证 MemberInfo.Name 从 Patch 调用回 Host 后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_MemberInfo_Name()
    {
        MemberInfo mi = typeof(PatchHelper).GetMethod("Multiply");
        Assert.Equal("Multiply", mi.Name);
    }

    /// <summary>
    /// Patch 侧：验证 FieldInfo.GetValue 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_FieldInfo_GetValue()
    {
        var obj = new PatchHelper();
        FieldInfo fi = typeof(PatchHelper).GetField("Value");
        object val = fi.GetValue(obj);
        Assert.Equal(7, val);
    }
}
