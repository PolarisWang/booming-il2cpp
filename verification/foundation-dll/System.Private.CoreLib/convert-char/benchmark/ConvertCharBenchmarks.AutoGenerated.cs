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

// Auto-generated benchmark skeletons for ConvertCharBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ConvertCharBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { _ = Convert.ToChar(true); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { _ = Convert.ToChar((byte)42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { _ = Convert.ToChar('A'); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { _ = Convert.ToChar(DateTime.UtcNow); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { _ = Convert.ToChar(42m); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { _ = Convert.ToChar(42.0); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { _ = Convert.ToChar((short)42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { _ = Convert.ToChar(42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { _ = Convert.ToChar(42L); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { _ = Convert.ToChar(42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { _ = Convert.ToChar(42, null!); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { _ = Convert.ToChar((sbyte)42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { _ = Convert.ToChar(42.0f); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { _ = Convert.ToChar("hello"); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { _ = Convert.ToChar("hello", null!); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { _ = Convert.ToChar((ushort)42); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { _ = Convert.ToChar(42u); }
    // Purpose: Benchmark native-runtime performance of Convert.ToChar with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { _ = Convert.ToChar(42uL); }}
