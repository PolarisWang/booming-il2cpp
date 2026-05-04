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
/// 反射模块 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionModulePatch
{
    /// <summary>
    /// Patch 侧：验证 Module.Name 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ModuleName()
    {
        Module m = typeof(ulong).Module;
        Assert.False(string.IsNullOrEmpty(m.Name));
    }

    /// <summary>
    /// Patch 侧：验证 Module.Assembly 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ModuleAssembly()
    {
        Module m = typeof(decimal).Module;
        Assembly asm = m.Assembly;
        Assert.NotNull(asm);
    }

    /// <summary>
    /// Patch 侧：验证 Module.GetTypes 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ModuleGetTypes()
    {
        Module m = typeof(sbyte).Module;
        Type[] types = m.GetTypes();
        Assert.True(types.Length > 0);
    }

    /// <summary>
    /// Patch 侧：验证 Module.FullyQualifiedName 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ModuleFullyQualifiedName()
    {
        Module m = typeof(ushort).Module;
        Assert.False(string.IsNullOrEmpty(m.FullyQualifiedName));
    }
}
