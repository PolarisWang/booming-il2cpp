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

// Auto-generated benchmark skeletons for GlobalizationCultureBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class GlobalizationCultureBenchmarks
{
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_CurrentCulture with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo() { _ = CultureInfo.CurrentCulture; }
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_InvariantCulture with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo() { _ = CultureInfo.InvariantCulture; }
    // Purpose: Benchmark native-runtime performance of CultureInfo.GetCultureInfo with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String() { }
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String() { _ = CultureInfo.InvariantCulture.Name; }
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_DisplayName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String() { _ = CultureInfo.InvariantCulture.DisplayName; }
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_DateTimeFormat with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo() { _ = CultureInfo.InvariantCulture.DateTimeFormat; }
    // Purpose: Benchmark native-runtime performance of CultureInfo.get_NumberFormat with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo() { _ = CultureInfo.InvariantCulture.NumberFormat; }
    // Purpose: Benchmark native-runtime performance of CompareInfo.Compare with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }
    // Purpose: Benchmark native-runtime performance of CompareInfo.Compare with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }
    // Purpose: Benchmark native-runtime performance of CompareInfo.IndexOf with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }
    // Purpose: Benchmark native-runtime performance of TextInfo.ToUpper with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello"); }
    // Purpose: Benchmark native-runtime performance of TextInfo.ToLower with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToLower("hello"); }
    // Purpose: Benchmark native-runtime performance of TextInfo.get_CultureName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.CultureName; }}
