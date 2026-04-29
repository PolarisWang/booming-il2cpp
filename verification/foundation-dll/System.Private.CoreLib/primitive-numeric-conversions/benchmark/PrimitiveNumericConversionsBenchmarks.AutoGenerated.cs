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

// Auto-generated benchmark skeletons for PrimitiveNumericConversionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class PrimitiveNumericConversionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Convert.ToBoolean with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String() { _ = Convert.ToBoolean("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToByte with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String() { _ = Convert.ToByte("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToInt16 with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String() { _ = Convert.ToInt16("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToInt32 with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String() { _ = Convert.ToInt32("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToInt64 with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String() { _ = Convert.ToInt64("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToSingle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String() { _ = Convert.ToSingle("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToDouble with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String() { _ = Convert.ToDouble("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToDecimal with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String() { _ = Convert.ToDecimal("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32() { _ = Convert.ToString(42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToString_System_String_System_Double() { _ = Convert.ToString(42.0); }
    // Purpose: Benchmark native-runtime performance of Convert.ToDecimal with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double() { _ = Convert.ToDecimal(42.0); }
    // Purpose: Benchmark native-runtime performance of Convert.ToInt32 with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double() { _ = Convert.ToInt32(42.0); }
    // Purpose: Benchmark native-runtime performance of Int32.Parse with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String() { _ = Int32.Parse("hello"); }
    // Purpose: Benchmark native-runtime performance of Int64.Parse with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String() { _ = Int64.Parse("hello"); }
    // Purpose: Benchmark native-runtime performance of Double.Parse with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Double_Parse_System_Double_System_String() { _ = Double.Parse("hello"); }}
