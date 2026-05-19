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

// Benchmark stubs for family/System.Private.CoreLib/math/numerics
public static class MathNumericsBenchmarks
{
    // Benchmark Math.Abs native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32() { _ = Math.Abs(42); }
    // Benchmark Math.Abs native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Abs_System_Double_System_Double() { _ = Math.Abs(42.0); }
    // Benchmark Math.Ceiling native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double() { _ = Math.Ceiling(42.0); }
    // Benchmark Math.Floor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Floor_System_Double_System_Double() { _ = Math.Floor(42.0); }
    // Benchmark Math.Round native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double() { _ = Math.Round(42.0); }
    // Benchmark Math.Round native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32() { _ = Math.Round(42.0); }
    // Benchmark Math.Max native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32() { _ = Math.Max(42, 42); }
    // Benchmark Math.Max native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double() { _ = Math.Max(42.0, 42.0); }
    // Benchmark Math.Min native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32() { _ = Math.Min(42, 42); }
    // Benchmark Math.Sqrt native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double() { _ = Math.Sqrt(42.0); }
    // Benchmark Math.Pow native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double() { _ = Math.Pow(42.0, 42.0); }
    // Benchmark Math.Sin native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Sin_System_Double_System_Double() { _ = Math.Sin(42.0); }
    // Benchmark Math.Cos native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Cos_System_Double_System_Double() { _ = Math.Cos(42.0); }
    // Benchmark Math.BigMul native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32() { _ = Math.BigMul(42, 42); }
    // Benchmark BitConverter.GetBytes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32() { _ = BitConverter.GetBytes(42); }
    // Benchmark BitConverter.GetBytes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double() { _ = BitConverter.GetBytes(42.0); }
    // Benchmark BitConverter.ToInt32 native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32() { _ = BitConverter.ToInt32(new byte[4], 0); }
    // Benchmark BitConverter.ToDouble native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32() { _ = BitConverter.ToDouble(new byte[8], 0); }}
