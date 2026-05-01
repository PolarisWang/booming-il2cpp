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

// Auto-generated hot-update skeletons for FamilySystem.formats.asn1DecoderHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.formats.asn1DecoderHotUpdate
{
    // Purpose: Verify AsnDecoder.AsnDecoder: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::AsnDecoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_AsnDecoder_Void_System_Object() { _ = AsnDecoder.AsnDecoder:(42); }
    // Purpose: Verify AsnDecoder.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Property_System_Void() { AsnDecoder.Property; }
    // Purpose: Verify AsnDecoder.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Method1_System_Void_P0() { AsnDecoder.Method1(null!); }
    // Purpose: Verify AsnDecoder.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Method2_System_Void_P0_P1() { AsnDecoder.Method2(null!, null!); }
    // Purpose: Verify AsnDecoder.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Validate_System_Void_P0() { AsnDecoder.Validate(null!); }
    // Purpose: Verify AsnDecoder.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Value_System_Void() { AsnDecoder.Value; }
    // Purpose: Verify AsnDecoder.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Value_System_Void_P0() { AsnDecoder.Value = null!; }
    // Purpose: Verify AsnDecoder.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Count_System_Void() { AsnDecoder.Count; }
    // Purpose: Verify AsnDecoder.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Count_System_Void_P0() { AsnDecoder.Count = null!; }
    // Purpose: Verify AsnDecoder.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_IsEmpty_System_Void() { AsnDecoder.IsEmpty; }
    // Purpose: Verify AsnDecoder.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_IsEmpty_System_Void_P0() { AsnDecoder.IsEmpty = null!; }
    // Purpose: Verify AsnDecoder.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Key_System_Void() { AsnDecoder.Key; }
    // Purpose: Verify AsnDecoder.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Key_System_Void_P0() { AsnDecoder.Key = null!; }}
