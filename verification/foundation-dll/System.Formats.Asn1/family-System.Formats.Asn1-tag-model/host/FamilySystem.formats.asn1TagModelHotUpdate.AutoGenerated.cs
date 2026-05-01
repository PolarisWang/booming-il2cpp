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

// Auto-generated hot-update skeletons for FamilySystem.formats.asn1TagModelHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.formats.asn1TagModelHotUpdate
{
    // Purpose: Verify Asn1Tag.Asn1Tag: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Asn1Tag::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Asn1Tag_Void_System_Object() { _ = Asn1Tag.Asn1Tag:(42); }
    // Purpose: Verify Asn1Tag.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Property_System_Void() { Asn1Tag.Property; }
    // Purpose: Verify Asn1Tag.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Method1_System_Void_P0() { Asn1Tag.Method1(null!); }
    // Purpose: Verify Asn1Tag.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Method2_System_Void_P0_P1() { Asn1Tag.Method2(null!, null!); }
    // Purpose: Verify Asn1Tag.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Validate_System_Void_P0() { Asn1Tag.Validate(null!); }
    // Purpose: Verify TagClass.TagClass: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::TagClass::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_TagClass_Void_System_Object() { _ = TagClass.TagClass:(42); }
    // Purpose: Verify TagClass.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Property_System_Void() { TagClass.Property; }
    // Purpose: Verify TagClass.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_Method1_System_Void_P0() { TagClass.Method1(null!); }
    // Purpose: Verify TagClass.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_Method2_System_Void_P0_P1() { TagClass.Method2(null!, null!); }
    // Purpose: Verify TagClass.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_Validate_System_Void_P0() { TagClass.Validate(null!); }
    // Purpose: Verify UniversalTagNumber.UniversalTagNumber: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::UniversalTagNumber::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_UniversalTagNumber_Void_System_Object() { _ = UniversalTagNumber.UniversalTagNumber:(42); }
    // Purpose: Verify UniversalTagNumber.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Property_System_Void() { UniversalTagNumber.Property; }
    // Purpose: Verify UniversalTagNumber.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Method1_System_Void_P0() { UniversalTagNumber.Method1(null!); }
    // Purpose: Verify UniversalTagNumber.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Method2_System_Void_P0_P1() { UniversalTagNumber.Method2(null!, null!); }
    // Purpose: Verify UniversalTagNumber.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Validate_System_Void_P0() { UniversalTagNumber.Validate(null!); }
    // Purpose: Verify Asn1Tag.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Value_System_Void() { Asn1Tag.Value; }
    // Purpose: Verify Asn1Tag.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Value_System_Void_P0() { Asn1Tag.Value = null!; }
    // Purpose: Verify Asn1Tag.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Count_System_Void() { Asn1Tag.Count; }
    // Purpose: Verify Asn1Tag.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Count_System_Void_P0() { Asn1Tag.Count = null!; }
    // Purpose: Verify Asn1Tag.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_IsEmpty_System_Void() { Asn1Tag.IsEmpty; }
    // Purpose: Verify Asn1Tag.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_IsEmpty_System_Void_P0() { Asn1Tag.IsEmpty = null!; }
    // Purpose: Verify Asn1Tag.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Key_System_Void() { Asn1Tag.Key; }
    // Purpose: Verify Asn1Tag.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Key_System_Void_P0() { Asn1Tag.Key = null!; }
    // Purpose: Verify TagClass.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Value_System_Void() { TagClass.Value; }
    // Purpose: Verify TagClass.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Value_System_Void_P0() { TagClass.Value = null!; }
    // Purpose: Verify TagClass.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Count_System_Void() { TagClass.Count; }
    // Purpose: Verify TagClass.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Count_System_Void_P0() { TagClass.Count = null!; }
    // Purpose: Verify TagClass.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_get_IsEmpty_System_Void() { TagClass.IsEmpty; }
    // Purpose: Verify TagClass.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_set_IsEmpty_System_Void_P0() { TagClass.IsEmpty = null!; }
    // Purpose: Verify TagClass.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Key_System_Void() { TagClass.Key; }
    // Purpose: Verify TagClass.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Key_System_Void_P0() { TagClass.Key = null!; }
    // Purpose: Verify UniversalTagNumber.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Value_System_Void() { UniversalTagNumber.Value; }
    // Purpose: Verify UniversalTagNumber.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Value_System_Void_P0() { UniversalTagNumber.Value = null!; }
    // Purpose: Verify UniversalTagNumber.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Count_System_Void() { UniversalTagNumber.Count; }
    // Purpose: Verify UniversalTagNumber.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Count_System_Void_P0() { UniversalTagNumber.Count = null!; }
    // Purpose: Verify UniversalTagNumber.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_IsEmpty_System_Void() { UniversalTagNumber.IsEmpty; }
    // Purpose: Verify UniversalTagNumber.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_IsEmpty_System_Void_P0() { UniversalTagNumber.IsEmpty = null!; }
    // Purpose: Verify UniversalTagNumber.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Key_System_Void() { UniversalTagNumber.Key; }
    // Purpose: Verify UniversalTagNumber.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Key_System_Void_P0() { UniversalTagNumber.Key = null!; }}
