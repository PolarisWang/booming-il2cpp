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
/// 反射模块测试 — 测试 Module 元数据查询 API（手动编写）
/// 涵盖：Name, FullyQualifiedName, Assembly, GetTypes, GetCustomAttributes
/// </summary>
public class ReflectionModule
{
    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：Module.Name 属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ModuleName()
    {
        Module m = typeof(object).Module;
        _ = m.Name;
    }

    /// <summary>
    /// 基准测试：Module.Assembly 属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ModuleAssembly()
    {
        Module m = typeof(int).Module;
        _ = m.Assembly;
    }

    /// <summary>
    /// 基准测试：Module.GetTypes 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ModuleGetTypes()
    {
        Module m = typeof(byte).Module;
        Type[] types = m.GetTypes();
        _ = types.Length;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 Module.Name 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ModuleName()
    {
        Module m = typeof(long).Module;
        Assert.False(string.IsNullOrEmpty(m.Name));
    }

    /// <summary>
    /// 热更新主机侧：验证 Module.Assembly 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ModuleAssembly()
    {
        Module m = typeof(short).Module;
        Assembly asm = m.Assembly;
        Assert.NotNull(asm);
    }

    /// <summary>
    /// 热更新主机侧：验证 Module.GetTypes 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ModuleGetTypes()
    {
        Module m = typeof(float).Module;
        Type[] types = m.GetTypes();
        Assert.True(types.Length > 0);
    }
}
