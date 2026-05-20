using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/primitive/numeric-conversions
public static class PrimitiveNumericConversionsBenchmarks
{
    // Benchmark Convert.ToBoolean native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String() { _ = Convert.ToBoolean("true"); }
    // Benchmark Convert.ToByte native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String() { _ = Convert.ToByte("42"); }
    // Benchmark Convert.ToInt16 native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String() { _ = Convert.ToInt16("42"); }
    // Benchmark Convert.ToInt32 native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String() { _ = Convert.ToInt32("42"); }
    // Benchmark Convert.ToInt64 native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String() { _ = Convert.ToInt64("42"); }
    // Benchmark Convert.ToSingle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String() { _ = Convert.ToSingle("42"); }
    // Benchmark Convert.ToDouble native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String() { _ = Convert.ToDouble("42"); }
    // Benchmark Convert.ToDecimal native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String() { _ = Convert.ToDecimal("42"); }
    // Benchmark Convert.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32() { _ = Convert.ToString(42); }
    // Benchmark Convert.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Double() { _ = Convert.ToString(42.0); }
    // Benchmark Convert.ToDecimal native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double() { _ = Convert.ToDecimal(42.0); }
    // Benchmark Convert.ToInt32 native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double() { _ = Convert.ToInt32(42.0); }
    // Benchmark Int32.Parse native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String() { _ = int.Parse("42"); }
    // Benchmark Int64.Parse native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String() { _ = long.Parse("42"); }
    // Benchmark Double.Parse native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Double_Parse_System_Double_System_String() { _ = double.Parse("42"); }}
