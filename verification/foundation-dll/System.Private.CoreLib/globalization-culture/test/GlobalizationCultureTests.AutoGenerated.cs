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
using Xunit;

// Auto-generated skeleton for Globalization Culture (family/System.Private.CoreLib/globalization/culture).
public partial class GlobalizationCultureTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo()
    {
    _ = CultureInfo.CurrentCulture;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo()
    {
    _ = CultureInfo.InvariantCulture;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — GetCultureInfo with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String()
    {
    // TODO: needs-manual — GetCultureInfo with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String()
    {
    var result = CultureInfo.InvariantCulture.Name;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String()
    {
    var result = CultureInfo.InvariantCulture.DisplayName;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo()
    {
    _ = CultureInfo.InvariantCulture.DateTimeFormat;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo()
    {
    _ = CultureInfo.InvariantCulture.NumberFormat;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz()
    {
    _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.ToLower("hello");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String()
    {
    var result = CultureInfo.InvariantCulture.TextInfo.CultureName;
    Xunit.Assert.NotNull((object)result);
    }}
