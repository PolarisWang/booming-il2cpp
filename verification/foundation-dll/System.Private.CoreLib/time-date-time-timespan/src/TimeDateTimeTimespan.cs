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

public partial class TimeDateTimeTimespan
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — DateTime / TimeSpan
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DateTime_Ctor_YMD()
    {
        _ = new DateTime(2024, 1, 1);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DateTime_AddDays()
    {
        _ = new DateTime(2024, 1, 1).AddDays(10);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_TimeSpan_FromDays()
    {
        _ = TimeSpan.FromDays(42.0);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — DateTime / TimeSpan
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DateTime_ToString()
    {
        _ = DateTime.UtcNow.ToString();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DateTime_AddHours()
    {
        _ = DateTime.UtcNow.AddHours(42.0);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_TimeSpan_Ctor_HMS()
    {
        _ = new TimeSpan(42, 42, 42);
    }
}
