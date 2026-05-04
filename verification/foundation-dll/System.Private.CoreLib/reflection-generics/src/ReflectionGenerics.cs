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
/// 反射泛型测试 — 测试泛型类型元数据查询和构造 API（手动编写）
/// 涵盖：MakeGenericType, GetGenericTypeDefinition, GetGenericArguments, IsGenericType, ContainsGenericParameters 等
/// </summary>
public class ReflectionGenerics
{
    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：MakeGenericType 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MakeGenericType()
    {
        Type t = typeof(List<>).MakeGenericType(typeof(int));
        _ = t.FullName;
    }

    /// <summary>
    /// 基准测试：GetGenericTypeDefinition 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetGenericTypeDefinition()
    {
        Type t = typeof(Dictionary<string, object>).GetGenericTypeDefinition();
        _ = t.Name;
    }

    /// <summary>
    /// 基准测试：GetGenericArguments 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetGenericArguments()
    {
        Type[] args = typeof(Dictionary<string, object>).GetGenericArguments();
        _ = args.Length;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 MakeGenericType 在热更新后正确构造
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_MakeGenericType()
    {
        Type t = typeof(Nullable<>).MakeGenericType(typeof(DateTime));
        Assert.Equal(typeof(DateTime?), t);
    }

    /// <summary>
    /// 热更新主机侧：验证 GetGenericTypeDefinition 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetGenericTypeDefinition()
    {
        Type t = typeof(IComparable<int>).GetGenericTypeDefinition();
        Assert.Equal(typeof(IComparable<>), t);
    }

    /// <summary>
    /// 热更新主机侧：验证 IsGenericType 在热更新后正确判断
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_IsGenericType()
    {
        Assert.True(typeof(HashSet<int>).IsGenericType);
        Assert.False(typeof(String).IsGenericType);
    }
}
