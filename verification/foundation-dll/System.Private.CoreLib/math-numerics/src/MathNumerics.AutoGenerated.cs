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

// Auto-generated skeleton for Math Numerics (family/System.Private.CoreLib/math/numerics).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class MathNumerics
{
    // Math.Abs
    [MethodSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32()
    {
    var result = Math.Abs(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32() { _ = Math.Abs(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32() { _ = Math.Abs(42); }

    // Math.Abs
    [MethodSubjectId("System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Abs_System_Double_System_Double()
    {
    var result = Math.Abs(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Abs_System_Double_System_Double() { _ = Math.Abs(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Abs_System_Double_System_Double() { _ = Math.Abs(42.0); }

    // Math.Ceiling
    [MethodSubjectId("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double()
    {
    var result = Math.Ceiling(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double() { _ = Math.Ceiling(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double() { _ = Math.Ceiling(42.0); }

    // Math.Floor
    [MethodSubjectId("System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Floor_System_Double_System_Double()
    {
    var result = Math.Floor(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Floor_System_Double_System_Double() { _ = Math.Floor(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Floor_System_Double_System_Double() { _ = Math.Floor(42.0); }

    // Math.Round
    [MethodSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Round_System_Double_System_Double()
    {
    var result = Math.Round(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double() { _ = Math.Round(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Round_System_Double_System_Double() { _ = Math.Round(42.0); }

    // Math.Round
    [MethodSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32()
    {
    var result = Math.Round(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32() { _ = Math.Round(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32() { _ = Math.Round(42.0); }

    // Math.Max
    [MethodSubjectId("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32()
    {
    var result = Math.Max(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32() { _ = Math.Max(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32() { _ = Math.Max(42, 42); }

    // Math.Max
    [MethodSubjectId("System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double()
    {
    var result = Math.Max(42.0, 42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double() { _ = Math.Max(42.0, 42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double() { _ = Math.Max(42.0, 42.0); }

    // Math.Min
    [MethodSubjectId("System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32()
    {
    var result = Math.Min(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32() { _ = Math.Min(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32() { _ = Math.Min(42, 42); }

    // Math.Sqrt
    [MethodSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double()
    {
    var result = Math.Sqrt(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double() { _ = Math.Sqrt(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double() { _ = Math.Sqrt(42.0); }

    // Math.Pow
    [MethodSubjectId("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double()
    {
    var result = Math.Pow(42.0, 42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double() { _ = Math.Pow(42.0, 42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double() { _ = Math.Pow(42.0, 42.0); }

    // Math.Sin
    [MethodSubjectId("System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Sin_System_Double_System_Double()
    {
    var result = Math.Sin(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Sin_System_Double_System_Double() { _ = Math.Sin(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Sin_System_Double_System_Double() { _ = Math.Sin(42.0); }

    // Math.Cos
    [MethodSubjectId("System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_Cos_System_Double_System_Double()
    {
    var result = Math.Cos(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_Cos_System_Double_System_Double() { _ = Math.Cos(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_Cos_System_Double_System_Double() { _ = Math.Cos(42.0); }

    // Math.BigMul
    [MethodSubjectId("System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32()
    {
    var result = Math.BigMul(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32() { _ = Math.BigMul(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32() { _ = Math.BigMul(42, 42); }

    // BitConverter.GetBytes
    [MethodSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32()
    {
    var result = BitConverter.GetBytes(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32() { _ = BitConverter.GetBytes(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32() { _ = BitConverter.GetBytes(42); }

    // BitConverter.GetBytes
    [MethodSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double()
    {
    var result = BitConverter.GetBytes(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double() { _ = BitConverter.GetBytes(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double() { _ = BitConverter.GetBytes(42.0); }

    // BitConverter.ToInt32
    [MethodSubjectId("System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32()
    {
    // TODO: needs-manual — ToInt32 with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32() { }

    // BitConverter.ToDouble
    [MethodSubjectId("System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32()
    {
    // TODO: needs-manual — ToDouble with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    public static void Benchmark_System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32() { }
}
