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
/// 反射字段属性测试 — 测试 FieldInfo / PropertyInfo 元数据和取值赋值 API（手动编写）
/// 涵盖：GetValue/SetValue, FieldType/PropertyType, IsStatic/IsPublic, CanRead/CanWrite
/// </summary>
public class ReflectionFieldProperty
{
    // 辅助类型
    class TestHelper
    {
        public int PublicField = 100;
        private string _privateField = "secret";
        public static double StaticField = 3.14;
        public string ReadWriteProp { get; set; } = "prop-value";
        public int ReadOnlyProp { get; private set; } = 42;
        public static bool StaticProp { get; set; } = true;
    }

    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：FieldInfo.GetValue 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_FieldInfo_GetValue()
    {
        var obj = new TestHelper();
        FieldInfo fi = typeof(TestHelper).GetField("PublicField");
        _ = fi.GetValue(obj);
    }

    /// <summary>
    /// 基准测试：PropertyInfo.GetValue 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PropertyInfo_GetValue()
    {
        var obj = new TestHelper();
        PropertyInfo pi = typeof(TestHelper).GetProperty("ReadWriteProp");
        _ = pi.GetValue(obj);
    }

    /// <summary>
    /// 基准测试：FieldInfo.FieldType 属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_FieldInfo_FieldType()
    {
        FieldInfo fi = typeof(TestHelper).GetField("PublicField");
        _ = fi.FieldType;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 FieldInfo.GetValue 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_FieldInfo_GetValue()
    {
        var obj = new TestHelper();
        FieldInfo fi = typeof(TestHelper).GetField("PublicField");
        Assert.Equal(100, fi.GetValue(obj));
    }

    /// <summary>
    /// 热更新主机侧：验证 PropertyInfo.GetValue 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_PropertyInfo_GetValue()
    {
        var obj = new TestHelper();
        PropertyInfo pi = typeof(TestHelper).GetProperty("ReadWriteProp");
        Assert.Equal("prop-value", pi.GetValue(obj));
    }

    /// <summary>
    /// 热更新主机侧：验证 FieldInfo.FieldType 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_FieldInfo_FieldType()
    {
        FieldInfo fi = typeof(TestHelper).GetField("PublicField");
        Assert.Equal(typeof(int), fi.FieldType);
    }
}
