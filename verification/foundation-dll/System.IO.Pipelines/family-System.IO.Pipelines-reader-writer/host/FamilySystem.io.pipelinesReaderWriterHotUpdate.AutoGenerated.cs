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

// Auto-generated hot-update skeletons for FamilySystem.io.pipelinesReaderWriterHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.io.pipelinesReaderWriterHotUpdate
{
    // Purpose: Verify PipeReader.PipeReader: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::PipeReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_PipeReader_Void_System_Object() { _ = PipeReader.PipeReader:(42); }
    // Purpose: Verify PipeReader.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Property_System_Void() { PipeReader.Property; }
    // Purpose: Verify PipeReader.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method1_System_Void_P0() { PipeReader.Method1(null!); }
    // Purpose: Verify PipeReader.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method2_System_Void_P0_P1() { PipeReader.Method2(null!, null!); }
    // Purpose: Verify PipeReader.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Validate_System_Void_P0() { PipeReader.Validate(null!); }
    // Purpose: Verify PipeWriter.PipeWriter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::PipeWriter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_PipeWriter_Void_System_Object() { _ = PipeWriter.PipeWriter:(42); }
    // Purpose: Verify PipeWriter.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Property_System_Void() { PipeWriter.Property; }
    // Purpose: Verify PipeWriter.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method1_System_Void_P0() { PipeWriter.Method1(null!); }
    // Purpose: Verify PipeWriter.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method2_System_Void_P0_P1() { PipeWriter.Method2(null!, null!); }
    // Purpose: Verify PipeWriter.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Validate_System_Void_P0() { PipeWriter.Validate(null!); }
    // Purpose: Verify PipeReader.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Value_System_Void() { PipeReader.Value; }
    // Purpose: Verify PipeReader.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Value_System_Void_P0() { PipeReader.Value = null!; }
    // Purpose: Verify PipeReader.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Count_System_Void() { PipeReader.Count; }
    // Purpose: Verify PipeReader.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Count_System_Void_P0() { PipeReader.Count = null!; }
    // Purpose: Verify PipeReader.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_IsEmpty_System_Void() { PipeReader.IsEmpty; }
    // Purpose: Verify PipeReader.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_IsEmpty_System_Void_P0() { PipeReader.IsEmpty = null!; }
    // Purpose: Verify PipeReader.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Key_System_Void() { PipeReader.Key; }
    // Purpose: Verify PipeReader.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Key_System_Void_P0() { PipeReader.Key = null!; }
    // Purpose: Verify PipeWriter.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Value_System_Void() { PipeWriter.Value; }
    // Purpose: Verify PipeWriter.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Value_System_Void_P0() { PipeWriter.Value = null!; }
    // Purpose: Verify PipeWriter.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Count_System_Void() { PipeWriter.Count; }
    // Purpose: Verify PipeWriter.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Count_System_Void_P0() { PipeWriter.Count = null!; }
    // Purpose: Verify PipeWriter.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_IsEmpty_System_Void() { PipeWriter.IsEmpty; }
    // Purpose: Verify PipeWriter.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_IsEmpty_System_Void_P0() { PipeWriter.IsEmpty = null!; }
    // Purpose: Verify PipeWriter.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Key_System_Void() { PipeWriter.Key; }
    // Purpose: Verify PipeWriter.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Key_System_Void_P0() { PipeWriter.Key = null!; }}
