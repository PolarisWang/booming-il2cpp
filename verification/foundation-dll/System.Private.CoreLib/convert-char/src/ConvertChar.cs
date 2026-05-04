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

public partial class ConvertChar
{
    // ========== Benchmark 方法 ==========

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToChar_Boolean()
    {
        _ = Convert.ToChar(true);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToChar_Byte()
    {
        _ = Convert.ToChar((byte)65);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToChar_Int32()
    {
        _ = Convert.ToChar(65);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToChar_String()
    {
        _ = Convert.ToChar("A");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToChar_Double()
    {
        _ = Convert.ToChar(65.0);
    }

    // ========== 热更新方法 (Host -> Patch) ==========

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToChar_Boolean()
    {
        _ = Convert.ToChar(true);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToChar_Byte()
    {
        _ = Convert.ToChar((byte)65);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToChar_Int32()
    {
        _ = Convert.ToChar(65);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToChar_String()
    {
        _ = Convert.ToChar("A");
    }
}
