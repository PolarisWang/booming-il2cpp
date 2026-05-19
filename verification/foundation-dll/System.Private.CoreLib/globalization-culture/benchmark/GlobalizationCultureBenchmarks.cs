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

// Benchmark stubs for family/System.Private.CoreLib/globalization/culture
public static class GlobalizationCultureBenchmarks
{
    // Benchmark CultureInfo.get_CurrentCulture native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo() { _ = CultureInfo.CurrentCulture; }
    // Benchmark CultureInfo.get_InvariantCulture native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo() { _ = CultureInfo.InvariantCulture; }
    // Benchmark CultureInfo.GetCultureInfo native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String() { _ = CultureInfo.GetCultureInfo(""); }
    // Benchmark CultureInfo.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String() { _ = CultureInfo.InvariantCulture.Name; }
    // Benchmark CultureInfo.get_DisplayName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String() { _ = CultureInfo.InvariantCulture.DisplayName; }
    // Benchmark CultureInfo.get_DateTimeFormat native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo() { _ = CultureInfo.InvariantCulture.DateTimeFormat; }
    // Benchmark CultureInfo.get_NumberFormat native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo() { _ = CultureInfo.InvariantCulture.NumberFormat; }
    // Benchmark CompareInfo.Compare native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }
    // Benchmark CompareInfo.Compare native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }
    // Benchmark CompareInfo.IndexOf native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }
    // Benchmark TextInfo.ToUpper native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello"); }
    // Benchmark TextInfo.ToLower native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToLower("hello"); }
    // Benchmark TextInfo.get_CultureName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.CultureName; }}
