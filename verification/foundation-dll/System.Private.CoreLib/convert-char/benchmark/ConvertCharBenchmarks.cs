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

// Benchmark stubs for family/System.Private.CoreLib/convert/char
public static class ConvertCharBenchmarks
{
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { }
    // Benchmark Convert.ToChar native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { }}
