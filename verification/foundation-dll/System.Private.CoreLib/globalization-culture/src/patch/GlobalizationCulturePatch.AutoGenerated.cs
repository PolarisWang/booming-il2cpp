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

// Auto-generated patch-side skeletons for GlobalizationCulturePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class GlobalizationCulturePatch
{
    // Purpose: Verify CultureInfo.get_CurrentCulture executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo() { _ = CultureInfo.CurrentCulture; }
    // Purpose: Verify CultureInfo.get_InvariantCulture executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo() { _ = CultureInfo.InvariantCulture; }
    // Purpose: Verify CultureInfo.GetCultureInfo executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String() { }
    // Purpose: Verify CultureInfo.get_Name executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String() { _ = CultureInfo.InvariantCulture.Name; }
    // Purpose: Verify CultureInfo.get_DisplayName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String() { _ = CultureInfo.InvariantCulture.DisplayName; }
    // Purpose: Verify CultureInfo.get_DateTimeFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo() { _ = CultureInfo.InvariantCulture.DateTimeFormat; }
    // Purpose: Verify CultureInfo.get_NumberFormat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo() { _ = CultureInfo.InvariantCulture.NumberFormat; }
    // Purpose: Verify CompareInfo.Compare executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello"); }
    // Purpose: Verify CompareInfo.Compare executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globaliz() { _ = CultureInfo.InvariantCulture.CompareInfo.Compare("hello", "hello", System.Globalization.CompareOptions.None); }
    // Purpose: Verify CompareInfo.IndexOf executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String() { _ = CultureInfo.InvariantCulture.CompareInfo.IndexOf("hello", "hello"); }
    // Purpose: Verify TextInfo.ToUpper executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToUpper("hello"); }
    // Purpose: Verify TextInfo.ToLower executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.ToLower("hello"); }
    // Purpose: Verify TextInfo.get_CultureName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GlobalizationCulture)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String() { _ = CultureInfo.InvariantCulture.TextInfo.CultureName; }}
