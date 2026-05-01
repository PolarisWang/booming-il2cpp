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

// Auto-generated hot-update skeletons for FamilySystem.io.compression.brotliEncoderHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.io.compression.brotliEncoderHotUpdate
{
    // Purpose: Verify BrotliEncoder.BrotliEncoder: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::BrotliEncoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_BrotliEncoder_Void_System_Object() { _ = BrotliEncoder.BrotliEncoder:(42); }
    // Purpose: Verify BrotliEncoder.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Property_System_Void() { BrotliEncoder.Property; }
    // Purpose: Verify BrotliEncoder.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method1_System_Void_P0() { BrotliEncoder.Method1(null!); }
    // Purpose: Verify BrotliEncoder.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method2_System_Void_P0_P1() { BrotliEncoder.Method2(null!, null!); }
    // Purpose: Verify BrotliEncoder.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Validate_System_Void_P0() { BrotliEncoder.Validate(null!); }
    // Purpose: Verify BrotliEncoder.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Value_System_Void() { BrotliEncoder.Value; }
    // Purpose: Verify BrotliEncoder.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Value_System_Void_P0() { BrotliEncoder.Value = null!; }
    // Purpose: Verify BrotliEncoder.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Count_System_Void() { BrotliEncoder.Count; }
    // Purpose: Verify BrotliEncoder.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Count_System_Void_P0() { BrotliEncoder.Count = null!; }
    // Purpose: Verify BrotliEncoder.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_IsEmpty_System_Void() { BrotliEncoder.IsEmpty; }
    // Purpose: Verify BrotliEncoder.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_IsEmpty_System_Void_P0() { BrotliEncoder.IsEmpty = null!; }
    // Purpose: Verify BrotliEncoder.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Key_System_Void() { BrotliEncoder.Key; }
    // Purpose: Verify BrotliEncoder.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Key_System_Void_P0() { BrotliEncoder.Key = null!; }}
