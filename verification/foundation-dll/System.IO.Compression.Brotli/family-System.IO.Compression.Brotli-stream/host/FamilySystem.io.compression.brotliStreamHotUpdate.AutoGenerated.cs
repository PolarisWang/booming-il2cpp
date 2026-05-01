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

// Auto-generated hot-update skeletons for FamilySystem.io.compression.brotliStreamHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.io.compression.brotliStreamHotUpdate
{
    // Purpose: Verify BrotliStream.BrotliStream: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BrotliStream::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_BrotliStream_Void_System_Object() { _ = BrotliStream.BrotliStream:(42); }
    // Purpose: Verify BrotliStream.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Property_System_Void() { BrotliStream.Property; }
    // Purpose: Verify BrotliStream.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Method1_System_Void_P0() { BrotliStream.Method1(null!); }
    // Purpose: Verify BrotliStream.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Method2_System_Void_P0_P1() { BrotliStream.Method2(null!, null!); }
    // Purpose: Verify BrotliStream.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Validate_System_Void_P0() { BrotliStream.Validate(null!); }
    // Purpose: Verify BrotliStream.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Value_System_Void() { BrotliStream.Value; }
    // Purpose: Verify BrotliStream.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Value_System_Void_P0() { BrotliStream.Value = null!; }
    // Purpose: Verify BrotliStream.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Count_System_Void() { BrotliStream.Count; }
    // Purpose: Verify BrotliStream.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Count_System_Void_P0() { BrotliStream.Count = null!; }
    // Purpose: Verify BrotliStream.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_IsEmpty_System_Void() { BrotliStream.IsEmpty; }
    // Purpose: Verify BrotliStream.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_IsEmpty_System_Void_P0() { BrotliStream.IsEmpty = null!; }
    // Purpose: Verify BrotliStream.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Key_System_Void() { BrotliStream.Key; }
    // Purpose: Verify BrotliStream.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Key_System_Void_P0() { BrotliStream.Key = null!; }}
