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

public partial class EnumParsing
{
    // ========== Benchmark 方法 ==========

    [BenchmarkSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Parse()
    {
        _ = Enum.Parse(typeof(DayOfWeek), "Monday");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Parse_IgnoreCase()
    {
        _ = Enum.Parse(typeof(DayOfWeek), "monday", ignoreCase: true);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Enum::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToString()
    {
        _ = DayOfWeek.Monday.ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_IsDefined()
    {
        _ = Enum.IsDefined(typeof(DayOfWeek), "Monday");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetValues()
    {
        _ = Enum.GetValues(typeof(DayOfWeek));
    }

    // ========== 热更新方法 (Host -> Patch) ==========

    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Parse()
    {
        _ = Enum.Parse(typeof(DayOfWeek), "Monday");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToString()
    {
        _ = DayOfWeek.Monday.ToString();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_IsDefined()
    {
        _ = Enum.IsDefined(typeof(DayOfWeek), "Monday");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_GetValues()
    {
        _ = Enum.GetValues(typeof(DayOfWeek));
    }
}
