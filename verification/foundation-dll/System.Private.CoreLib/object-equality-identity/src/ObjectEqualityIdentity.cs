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

public partial class ObjectEqualityIdentity
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — 对象相等与标识
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Object_Equals_Static()
    {
        _ = Object.Equals(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Object_ReferenceEquals()
    {
        var a = new object();
        _ = Object.ReferenceEquals(a, a);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Object_GetType()
    {
        _ = new object().GetType();
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — 对象相等与标识
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Object_Equals_Instance()
    {
        _ = new object().Equals(42);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Object_GetHashCode()
    {
        _ = new object().GetHashCode();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Object_ToString()
    {
        _ = new object().ToString();
    }
}
