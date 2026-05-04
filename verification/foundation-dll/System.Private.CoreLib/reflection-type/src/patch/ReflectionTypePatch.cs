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
/// 反射类型查询 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionTypePatch
{
    /// <summary>
    /// Patch 侧：验证 Type.GetType 从 Patch 调用回 Host 后仍能正确加载类型
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetType_ByString()
    {
        Type t = Type.GetType("System.Decimal");
        Assert.NotNull(t);
    }

    /// <summary>
    /// Patch 侧：验证 typeof() 从 Patch 调用回 Host 后一致
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetTypeFromHandle()
    {
        Type t = Type.GetTypeFromHandle(typeof(DateTime).TypeHandle);
        Assert.Equal(typeof(DateTime), t);
    }

    /// <summary>
    /// Patch 侧：验证 Type 布尔属性从 Patch 调用回 Host 后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_TypeIsAbstract()
    {
        Assert.True(typeof(TextReader).IsAbstract);
        Assert.False(typeof(String).IsAbstract);
    }

    /// <summary>
    /// Patch 侧：验证 Name/FullyQualifiedName 从 Patch 调用回 Host 后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_TypeName()
    {
        Type t = typeof(TimeSpan);
        Assert.Equal("TimeSpan", t.Name);
        _ = t.Namespace;
    }
}
