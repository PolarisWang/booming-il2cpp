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
/// 反射激活 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionActivationPatch
{
    // 复现辅助类型
    class PatchHelper
    {
        public int Id;
        public PatchHelper() { Id = 1; }
        public PatchHelper(int id) { Id = id; }
    }

    /// <summary>
    /// Patch 侧：验证 CreateInstance<T>() 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CreateInstance_Generic()
    {
        var obj = Activator.CreateInstance<PatchHelper>();
        Assert.NotNull(obj);
        Assert.Equal(1, obj.Id);
    }

    /// <summary>
    /// Patch 侧：验证 CreateInstance(Type) 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CreateInstance_ByType()
    {
        object obj = Activator.CreateInstance(typeof(PatchHelper));
        Assert.NotNull(obj);
    }

    /// <summary>
    /// Patch 侧：验证 CreateInstance(Type, object[]) 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CreateInstance_WithArgs()
    {
        object obj = Activator.CreateInstance(typeof(PatchHelper), new object[] { 50 });
        var helper = (PatchHelper)obj;
        Assert.Equal(50, helper.Id);
    }

    /// <summary>
    /// Patch 侧：验证 ConstructorInfo.Invoke 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ConstructorInfo_Invoke()
    {
        ConstructorInfo ci = typeof(PatchHelper).GetConstructor(new[] { typeof(int) });
        object obj = ci.Invoke(new object[] { 33 });
        Assert.Equal(33, ((PatchHelper)obj).Id);
    }
}
