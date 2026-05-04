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

public partial class PrimitiveNumericConversions
{
    // ========== Benchmark 方法 ==========

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToInt32_String()
    {
        _ = Convert.ToInt32("12345");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToInt64_String()
    {
        _ = Convert.ToInt64("9876543210");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToDouble_String()
    {
        _ = Convert.ToDouble("3.14159");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ToString_Int32()
    {
        _ = Convert.ToString(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Int32_Parse()
    {
        _ = int.Parse("1024");
    }

    // ========== 热更新方法 (Host -> Patch) ==========

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToInt32_String()
    {
        _ = Convert.ToInt32("12345");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToInt64_String()
    {
        _ = Convert.ToInt64("9876543210");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToDouble_String()
    {
        _ = Convert.ToDouble("3.14159");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ToString_Int32()
    {
        _ = Convert.ToString(42);
    }
}
