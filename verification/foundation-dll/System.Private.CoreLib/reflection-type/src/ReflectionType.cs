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
/// 反射类型查询测试 — 测试 Type 元数据查询 API 的完整性（手动编写）
/// 涵盖：GetType, typeof(), Name/FullName/Namespace, IsAbstract/IsSealed/IsGenericType 等
/// </summary>
public partial class ReflectionType
{
    // ============================================================
    // Benchmark 方法（至少 3 个）
    // ============================================================

    /// <summary>
    /// 基准测试：Type.GetType(string) 性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetType_ByString()
    {
        Type t = Type.GetType("System.Int32");
        _ = t.Name;
    }

    /// <summary>
    /// 基准测试：typeof() + Name 属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_TypeOfAndName()
    {
        Type t = typeof(string);
        _ = t.Name;
    }

    /// <summary>
    /// 基准测试：Type 布尔属性访问性能基线
    /// </summary>
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_TypeIsAbstract()
    {
        Type t = typeof(Stream);
        _ = t.IsAbstract;
    }

    // ============================================================
    // Host 侧 HotUpdate 方法（至少 3 个，HostToPatch）
    // ============================================================

    /// <summary>
    /// 热更新主机侧：验证 Type.GetType 在热更新后仍能正确加载类型
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_GetType_ByString()
    {
        Type t = Type.GetType("System.Double");
        _ = t.Name;
    }

    /// <summary>
    /// 热更新主机侧：验证 typeof() 与 Name 在热更新后一致
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_TypeName()
    {
        Type t = typeof(Version);
        _ = t.Name;
    }

    /// <summary>
    /// 热更新主机侧：验证 FullName 在热更新后正确
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Host_TypeFullName()
    {
        Type t = typeof(Uri);
        _ = t.FullName;
    }
}
