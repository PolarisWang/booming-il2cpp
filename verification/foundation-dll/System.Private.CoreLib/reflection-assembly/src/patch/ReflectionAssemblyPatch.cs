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
/// 反射程序集查询 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionAssemblyPatch
{
    /// <summary>
    /// Patch 侧：验证 GetExecutingAssembly 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetExecutingAssembly()
    {
        Assembly asm = Assembly.GetExecutingAssembly();
        Assert.NotNull(asm);
    }

    /// <summary>
    /// Patch 侧：验证 FullName 从 Patch 调用回 Host 后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_FullName()
    {
        Assembly asm = typeof(long).Assembly;
        _ = asm.FullName;
    }

    /// <summary>
    /// Patch 侧：验证 GetName 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetName()
    {
        Assembly asm = typeof(float).Assembly;
        AssemblyName name = asm.GetName();
        Assert.NotNull(name.Name);
    }

    /// <summary>
    /// Patch 侧：验证 GetTypes 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetTypes()
    {
        Assembly asm = typeof(double).Assembly;
        Type[] types = asm.GetTypes();
        Assert.True(types.Length > 0);
    }
}
