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

// Auto-generated skeleton for Globalization Culture (family/System.Private.CoreLib/globalization/culture).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class GlobalizationCulture
{
    // CultureInfo.get_CurrentCulture
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo()
    {
    _ = CultureInfo.CurrentCulture;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo() { _ = CultureInfo.CurrentCulture; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo() { _ = CultureInfo.CurrentCulture; }

    // CultureInfo.get_InvariantCulture
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo()
    {
    _ = CultureInfo.InvariantCulture;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo() { _ = CultureInfo.InvariantCulture; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo() { _ = CultureInfo.InvariantCulture; }

    // CultureInfo.GetCultureInfo
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String()
    {
    // TODO: needs-manual — GetCultureInfo with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String() { }

    // CultureInfo.get_Name
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String()
    {
    var result = CultureInfo.InvariantCulture.Name;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String() { _ = CultureInfo.InvariantCulture.Name; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String() { _ = CultureInfo.InvariantCulture.Name; }

    // CultureInfo.get_DisplayName
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String()
    {
    var result = CultureInfo.InvariantCulture.DisplayName;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String() { _ = CultureInfo.InvariantCulture.DisplayName; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String() { _ = CultureInfo.InvariantCulture.DisplayName; }

    // CultureInfo.get_DateTimeFormat
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo()
    {
    _ = CultureInfo.InvariantCulture.DateTimeFormat;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo() { _ = CultureInfo.InvariantCulture.DateTimeFormat; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo() { _ = CultureInfo.InvariantCulture.DateTimeFormat; }

    // CultureInfo.get_NumberFormat
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo()
    {
    _ = CultureInfo.InvariantCulture.NumberFormat;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo() { _ = CultureInfo.InvariantCulture.NumberFormat; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo() { _ = CultureInfo.InvariantCulture.NumberFormat; }

    // CompareInfo.Compare
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }

    // CompareInfo.Compare
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz()
    {
    _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }

    // CompareInfo.IndexOf
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }

    // TextInfo.ToUpper
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello"); }

    // TextInfo.ToLower
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.ToLower("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToLower("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToLower("hello"); }

    // TextInfo.get_CultureName
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.CultureName;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.CultureName; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.CultureName; }
}
