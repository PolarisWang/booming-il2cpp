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

public partial class MathNumerics
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — Math 函数
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Math_Abs_Int()
    {
        _ = Math.Abs(-42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Math_Sqrt()
    {
        _ = Math.Sqrt(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Math_Pow()
    {
        _ = Math.Pow(2.0, 8.0);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — Math 函数
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Math_Max()
    {
        _ = Math.Max(42, 100);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Math_Ceiling()
    {
        _ = Math.Ceiling(3.2);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_BitConverter_GetBytes()
    {
        _ = BitConverter.GetBytes(42);
    }
}
