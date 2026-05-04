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
/// 反射激活测试 — 测试 Activator.CreateInstance 和 ConstructorInfo.Invoke（手动编写）
/// 涵盖：CreateInstance<T>(), CreateInstance(Type), CreateInstance(Type, object[]), ConstructorInfo.Invoke
/// </summary>
public class ReflectionActivation
{
    // 辅助类型
    class TestHelper
    {
        public int Value;
        public TestHelper() { Value = 1; }
        public TestHelper(int x) { Value = x; }
        public TestHelper(int x, string y) { Value = x + y.Length; }
    }

    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：CreateInstance<T>() 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CreateInstance_Generic()
    {
        _ = Activator.CreateInstance<TestHelper>();
    }

    /// <summary>
    /// 基准测试：CreateInstance(Type) 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CreateInstance_ByType()
    {
        _ = Activator.CreateInstance(typeof(TestHelper));
    }

    /// <summary>
    /// 基准测试：CreateInstance(Type, object[]) 带参数性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CreateInstance_WithArgs()
    {
        _ = Activator.CreateInstance(typeof(TestHelper), new object[] { 7 });
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 CreateInstance<T>() 在热更新后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_CreateInstance_Generic()
    {
        var obj = Activator.CreateInstance<Version>();
        Assert.NotNull(obj);
    }

    /// <summary>
    /// 热更新主机侧：验证 CreateInstance(Type) 在热更新后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_CreateInstance_ByType()
    {
        object obj = Activator.CreateInstance(typeof(TestHelper));
        Assert.NotNull(obj);
    }

    /// <summary>
    /// 热更新主机侧：验证 ConstructorInfo.Invoke 在热更新后正常
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ConstructorInfo_Invoke()
    {
        ConstructorInfo ci = typeof(Uri).GetConstructor(new[] { typeof(string) });
        object obj = ci.Invoke(new object[] { "http://example.com" });
        Assert.NotNull(obj);
    }
}
