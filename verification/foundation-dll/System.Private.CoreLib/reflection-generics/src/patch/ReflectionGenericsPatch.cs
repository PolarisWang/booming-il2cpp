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
/// 反射泛型 — Patch 侧热更新验证（从 Patch 调用回 Host）
/// </summary>
public static class ReflectionGenericsPatch
{
    /// <summary>
    /// Patch 侧：验证 MakeGenericType 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_MakeGenericType()
    {
        Type t = typeof(List<>).MakeGenericType(typeof(double));
        Assert.True(t.IsConstructedGenericType);
    }

    /// <summary>
    /// Patch 侧：验证 GetGenericTypeDefinition 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetGenericTypeDefinition()
    {
        Type t = typeof(IEnumerable<byte>).GetGenericTypeDefinition();
        Assert.Equal(typeof(IEnumerable<>), t);
    }

    /// <summary>
    /// Patch 侧：验证 GetGenericArguments 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GetGenericArguments()
    {
        Type[] args = typeof(KeyValuePair<string, object>).GetGenericArguments();
        Assert.Equal(2, args.Length);
        Assert.Equal(typeof(string), args[0]);
    }

    /// <summary>
    /// Patch 侧：验证 ContainsGenericParameters 从 Patch 调用回 Host 后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ContainsGenericParameters()
    {
        Assert.True(typeof(IComparable<>).ContainsGenericParameters);
        Assert.False(typeof(IComparable<int>).ContainsGenericParameters);
    }
}
