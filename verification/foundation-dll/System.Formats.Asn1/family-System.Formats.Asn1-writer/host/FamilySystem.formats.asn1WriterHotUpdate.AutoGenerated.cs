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

// Auto-generated hot-update skeletons for FamilySystem.formats.asn1WriterHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.formats.asn1WriterHotUpdate
{
    // Purpose: Verify AsnWriter.AsnWriter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::AsnWriter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_AsnWriter_Void_System_Object() { _ = AsnWriter.AsnWriter:(42); }
    // Purpose: Verify AsnWriter.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Property_System_Void() { AsnWriter.Property; }
    // Purpose: Verify AsnWriter.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Method1_System_Void_P0() { AsnWriter.Method1(null!); }
    // Purpose: Verify AsnWriter.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Method2_System_Void_P0_P1() { AsnWriter.Method2(null!, null!); }
    // Purpose: Verify AsnWriter.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Validate_System_Void_P0() { AsnWriter.Validate(null!); }
    // Purpose: Verify AsnWriter.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Value_System_Void() { AsnWriter.Value; }
    // Purpose: Verify AsnWriter.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Value_System_Void_P0() { AsnWriter.Value = null!; }
    // Purpose: Verify AsnWriter.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Count_System_Void() { AsnWriter.Count; }
    // Purpose: Verify AsnWriter.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Count_System_Void_P0() { AsnWriter.Count = null!; }
    // Purpose: Verify AsnWriter.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_IsEmpty_System_Void() { AsnWriter.IsEmpty; }
    // Purpose: Verify AsnWriter.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_IsEmpty_System_Void_P0() { AsnWriter.IsEmpty = null!; }
    // Purpose: Verify AsnWriter.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Key_System_Void() { AsnWriter.Key; }
    // Purpose: Verify AsnWriter.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Key_System_Void_P0() { AsnWriter.Key = null!; }}
