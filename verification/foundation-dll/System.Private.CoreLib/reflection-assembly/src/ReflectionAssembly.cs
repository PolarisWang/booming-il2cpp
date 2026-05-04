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
/// 反射程序集查询测试 — 测试 Assembly 元数据查询 API（手动编写）
/// 涵盖：GetExecutingAssembly, GetCallingAssembly, GetTypes, GetName, FullName, Location 等
/// </summary>
public class ReflectionAssembly
{
    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：GetExecutingAssembly 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetExecutingAssembly()
    {
        Assembly asm = Assembly.GetExecutingAssembly();
        _ = asm.FullName;
    }

    /// <summary>
    /// 基准测试：Assembly.FullName 属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_FullName()
    {
        Assembly asm = typeof(object).Assembly;
        _ = asm.FullName;
    }

    /// <summary>
    /// 基准测试：GetName 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetName()
    {
        Assembly asm = typeof(int).Assembly;
        AssemblyName name = asm.GetName();
        _ = name.Name;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 GetExecutingAssembly 在热更新后仍正常工作
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetExecutingAssembly()
    {
        Assembly asm = Assembly.GetExecutingAssembly();
        Assert.NotNull(asm);
    }

    /// <summary>
    /// 热更新主机侧：验证 FullName 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_FullName()
    {
        Assembly asm = typeof(byte).Assembly;
        Assert.False(string.IsNullOrEmpty(asm.FullName));
    }

    /// <summary>
    /// 热更新主机侧：验证 GetTypes 在热更新后仍返回类型数组
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetTypes()
    {
        Assembly asm = typeof(short).Assembly;
        Type[] types = asm.GetTypes();
        Assert.True(types.Length > 0);
    }
}
