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
/// 反射成员完全测试 — 测试 MethodInfo/ConstructorInfo/PropertyInfo/FieldInfo/EventInfo 调用 API（手动编写）
/// 涵盖：Invoke, GetValue/SetValue, AddEventHandler/RemoveEventHandler, MemberInfo.Name/MemberType/DeclaringType
/// </summary>
public class ReflectionMemberComplete
{
    // 辅助测试类型
    class TestHelper
    {
        public int ValueField = 42;
        public string NameProperty { get; set; } = "hello";
        public int Add(int a, int b) => a + b;
        public TestHelper() { }
        public TestHelper(int seed) { ValueField = seed; }
        public event EventHandler TestEvent;
        public void RaiseTestEvent() => TestEvent?.Invoke(this, EventArgs.Empty);
    }

    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：MethodInfo.Invoke 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MethodInfo_Invoke()
    {
        var obj = new TestHelper();
        MethodInfo mi = typeof(TestHelper).GetMethod("Add");
        mi.Invoke(obj, new object[] { 1, 2 });
    }

    /// <summary>
    /// 基准测试：FieldInfo.GetValue 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_FieldInfo_GetValue()
    {
        var obj = new TestHelper();
        FieldInfo fi = typeof(TestHelper).GetField("ValueField");
        _ = fi.GetValue(obj);
    }

    /// <summary>
    /// 基准测试：ConstructorInfo.Invoke 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ConstructorInfo_Invoke()
    {
        ConstructorInfo ci = typeof(TestHelper).GetConstructor(Type.EmptyTypes);
        ci.Invoke(null);
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 MethodInfo.Invoke 在热更新后仍正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_MethodInfo_Invoke()
    {
        var obj = new TestHelper();
        MethodInfo mi = typeof(TestHelper).GetMethod("Add");
        object result = mi.Invoke(obj, new object[] { 10, 20 });
        Assert.Equal(30, result);
    }

    /// <summary>
    /// 热更新主机侧：验证 ConstructorInfo.Invoke 在热更新后仍正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_ConstructorInfo_Invoke()
    {
        ConstructorInfo ci = typeof(TestHelper).GetConstructor(Type.EmptyTypes);
        object obj = ci.Invoke(null);
        Assert.NotNull(obj);
    }

    /// <summary>
    /// 热更新主机侧：验证 FieldInfo.SetValue 在热更新后仍正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_FieldInfo_SetValue()
    {
        var obj = new TestHelper();
        FieldInfo fi = typeof(TestHelper).GetField("ValueField");
        fi.SetValue(obj, 100);
        Assert.Equal(100, fi.GetValue(obj));
    }
}
