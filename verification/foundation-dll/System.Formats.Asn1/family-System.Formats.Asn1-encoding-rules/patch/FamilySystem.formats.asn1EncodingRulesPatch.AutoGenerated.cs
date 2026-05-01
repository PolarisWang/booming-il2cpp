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

// Auto-generated patch-side skeletons for FamilySystem.formats.asn1EncodingRulesPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.formats.asn1EncodingRulesPatch
{
    // Purpose: Verify AsnEncodingRules.AsnEncodingRules: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::AsnEncodingRules::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_AsnEncodingRules_Void_System_Object() { _ = AsnEncodingRules.AsnEncodingRules:(42); }
    // Purpose: Verify AsnEncodingRules.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Property_System_Void() { AsnEncodingRules.Property; }
    // Purpose: Verify AsnEncodingRules.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Method1_System_Void_P0() { AsnEncodingRules.Method1(null!); }
    // Purpose: Verify AsnEncodingRules.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Method2_System_Void_P0_P1() { AsnEncodingRules.Method2(null!, null!); }
    // Purpose: Verify AsnEncodingRules.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Validate_System_Void_P0() { AsnEncodingRules.Validate(null!); }
    // Purpose: Verify AsnEncodingRules.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Value_System_Void() { AsnEncodingRules.Value; }
    // Purpose: Verify AsnEncodingRules.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Value_System_Void_P0() { AsnEncodingRules.Value = null!; }
    // Purpose: Verify AsnEncodingRules.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Count_System_Void() { AsnEncodingRules.Count; }
    // Purpose: Verify AsnEncodingRules.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Count_System_Void_P0() { AsnEncodingRules.Count = null!; }
    // Purpose: Verify AsnEncodingRules.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_IsEmpty_System_Void() { AsnEncodingRules.IsEmpty; }
    // Purpose: Verify AsnEncodingRules.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_IsEmpty_System_Void_P0() { AsnEncodingRules.IsEmpty = null!; }
    // Purpose: Verify AsnEncodingRules.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Key_System_Void() { AsnEncodingRules.Key; }
    // Purpose: Verify AsnEncodingRules.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Key_System_Void_P0() { AsnEncodingRules.Key = null!; }}
