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

// Auto-generated hot-update skeletons for FamilySystem.formats.asn1ReaderHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.formats.asn1ReaderHotUpdate
{
    // Purpose: Verify AsnReader.AsnReader: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::AsnReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_AsnReader_Void_System_Object() { _ = AsnReader.AsnReader:(42); }
    // Purpose: Verify AsnReader.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Property_System_Void() { AsnReader.Property; }
    // Purpose: Verify AsnReader.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method1_System_Void_P0() { AsnReader.Method1(null!); }
    // Purpose: Verify AsnReader.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method2_System_Void_P0_P1() { AsnReader.Method2(null!, null!); }
    // Purpose: Verify AsnReader.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Validate_System_Void_P0() { AsnReader.Validate(null!); }
    // Purpose: Verify AsnReader.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Value_System_Void() { AsnReader.Value; }
    // Purpose: Verify AsnReader.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Value_System_Void_P0() { AsnReader.Value = null!; }
    // Purpose: Verify AsnReader.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Count_System_Void() { AsnReader.Count; }
    // Purpose: Verify AsnReader.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Count_System_Void_P0() { AsnReader.Count = null!; }
    // Purpose: Verify AsnReader.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_IsEmpty_System_Void() { AsnReader.IsEmpty; }
    // Purpose: Verify AsnReader.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_IsEmpty_System_Void_P0() { AsnReader.IsEmpty = null!; }
    // Purpose: Verify AsnReader.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Key_System_Void() { AsnReader.Key; }
    // Purpose: Verify AsnReader.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Key_System_Void_P0() { AsnReader.Key = null!; }}
