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
/// 反射参数信息测试 — 测试 ParameterInfo 元数据查询 API（手动编写）
/// 涵盖：Name, ParameterType, DefaultValue, HasDefaultValue, IsOptional, IsOut, GetParameters
/// </summary>
public partial class ReflectionParameters
{
    // 辅助类型
    class TestHelper
    {
        public void MethodWithParams(int id, string name, out bool flag, ref int refVal)
        {
            flag = true;
            refVal = 99;
        }
        public void MethodWithDefault(int optional = 42) { }
        public void MethodWithOutOnly(out int result) { result = 1; }
    }

    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：GetParameters 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetParameters()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithParams");
        ParameterInfo[] ps = mi.GetParameters();
        _ = ps.Length;
    }

    /// <summary>
    /// 基准测试：ParameterInfo.Name 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ParameterInfo_Name()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithParams");
        _ = mi.GetParameters()[0].Name;
    }

    /// <summary>
    /// 基准测试：ParameterInfo.HasDefaultValue 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_HasDefaultValue()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithDefault");
        _ = mi.GetParameters()[0].HasDefaultValue;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 GetParameters 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetParameters()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithParams");
        ParameterInfo[] ps = mi.GetParameters();
        _ = ps.Length;
    }

    /// <summary>
    /// 热更新主机侧：验证 ParameterInfo.Name 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ParameterName()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithDefault");
        _ = mi.GetParameters()[0].Name;
    }

    /// <summary>
    /// 热更新主机侧：验证 ParameterType 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ParameterType()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("MethodWithParams");
        _ = mi.GetParameters()[0].ParameterType;
    }
}
