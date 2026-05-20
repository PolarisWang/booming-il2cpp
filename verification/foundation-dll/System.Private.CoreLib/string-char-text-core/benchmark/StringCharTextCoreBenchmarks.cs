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

// Benchmark stubs for family/System.Private.CoreLib/string/char-text-core
public static class StringCharTextCoreBenchmarks
{
    // Benchmark String.IndexOf native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String() { _ = "hello".IndexOf("hello"); }
    // Benchmark String.IndexOf native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char() { _ = "hello".IndexOf('A'); }
    // Benchmark String.Substring native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32() { _ = "hello".Substring(1); }
    // Benchmark String.Substring native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32() { _ = "hello".Substring(1, 2); }
    // Benchmark String.Compare native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String() { _ = String.Compare("hello", "hello"); }
    // Benchmark String.Compare native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I() { _ = string.Compare("hello", 0, "world", 0, 3); }
    // Benchmark String.Concat native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String() { _ = String.Concat("hello", "hello"); }
    // Benchmark String.Concat native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String() { _ = String.Concat("hello", "hello", "hello"); }
    // Benchmark String.Format native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object() { _ = String.Format("hello", Array.Empty<System.Object>()); }
    // Benchmark String.StartsWith native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String() { _ = "hello".StartsWith("hello"); }
    // Benchmark String.Contains native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String() { _ = "hello".Contains("hello"); }
    // Benchmark String.Replace native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String() { _ = "hello".Replace("hello", "hello"); }
    // Benchmark String.Split native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Split_System_String_System_Char() { _ = "hello".Split('A'); }
    // Benchmark String.ToUpper native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_ToUpper_System_String() { _ = "hello".ToUpper(); }
    // Benchmark String.ToLower native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_ToLower_System_String() { _ = "hello".ToLower(); }
    // Benchmark String.Trim native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Trim_System_String() { _ = "hello".Trim(); }
    // Benchmark String.Join native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String() { _ = String.Join("hello", Array.Empty<System.String>()); }
    // Benchmark Char.IsDigit native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char() { _ = Char.IsDigit('A'); }
    // Benchmark Char.IsLetter native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char() { _ = Char.IsLetter('A'); }
    // Benchmark Char.IsWhiteSpace native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char() { _ = Char.IsWhiteSpace('A'); }}
