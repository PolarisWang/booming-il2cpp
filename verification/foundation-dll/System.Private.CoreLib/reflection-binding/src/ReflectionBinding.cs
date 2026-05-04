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
/// 反射绑定标志测试 — 测试 BindingFlags 查找与 MethodBase.Invoke/Type.InvokeMember（手动编写）
/// 涵盖：GetMethod/GetField/GetProperty with BindingFlags, MethodBase.Invoke with BindingFlags
/// </summary>
public class ReflectionBinding
{
    // 辅助类型
    class TestHelper
    {
        private int _hiddenField = 123;
        public int PublicField = 456;
        public int PublicProperty { get; set; } = 789;
        private static string _staticField = "static";
        public static string StaticMethod() => "static-result";
        private string PrivateMethod() => "private-result";
    }

    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：GetMethod with BindingFlags 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetMethod_WithBindingFlags()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("StaticMethod",
            BindingFlags.Public | BindingFlags.Static);
        _ = mi.Name;
    }

    /// <summary>
    /// 基准测试：GetField with BindingFlags 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetField_WithBindingFlags()
    {
        FieldInfo fi = typeof(TestHelper).GetField("_hiddenField",
            BindingFlags.NonPublic | BindingFlags.Instance);
        _ = fi.Name;
    }

    /// <summary>
    /// 基准测试：MethodBase.Invoke 默认方式性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MethodBase_Invoke()
    {
        var obj = new TestHelper();
        MethodInfo mi = typeof(TestHelper).GetMethod("StaticMethod",
            BindingFlags.Public | BindingFlags.Static);
        mi.Invoke(null, null);
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 GetMethod with BindingFlags 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetMethod_WithBindingFlags()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("PrivateMethod",
            BindingFlags.NonPublic | BindingFlags.Instance);
        Assert.NotNull(mi);
    }

    /// <summary>
    /// 热更新主机侧：验证 GetField with BindingFlags 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetField_WithBindingFlags()
    {
        FieldInfo fi = typeof(TestHelper).GetField("_hiddenField",
            BindingFlags.NonPublic | BindingFlags.Instance);
        Assert.NotNull(fi);
        Assert.Equal(123, fi.GetValue(new TestHelper()));
    }

    /// <summary>
    /// 热更新主机侧：验证 MethodBase.Invoke with BindingFlags 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_Invoke_WithBindingFlags()
    {
        MethodInfo mi = typeof(TestHelper).GetMethod("StaticMethod",
            BindingFlags.Public | BindingFlags.Static);
        object result = mi.Invoke(null, BindingFlags.Default, null,
            new object[] { }, CultureInfo.InvariantCulture);
        Assert.Equal("static-result", result);
    }
}
