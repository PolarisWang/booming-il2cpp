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

// Auto-generated patch-side skeletons for FamilySystem.io.compression.brotliDecoderPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.compression.brotliDecoderPatch
{
    // Purpose: Verify BrotliDecoder.BrotliDecoder: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::BrotliDecoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_BrotliDecoder_Void_System_Object() { _ = BrotliDecoder.BrotliDecoder:(42); }
    // Purpose: Verify BrotliDecoder.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Property_System_Void() { BrotliDecoder.Property; }
    // Purpose: Verify BrotliDecoder.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Method1_System_Void_P0() { BrotliDecoder.Method1(null!); }
    // Purpose: Verify BrotliDecoder.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Method2_System_Void_P0_P1() { BrotliDecoder.Method2(null!, null!); }
    // Purpose: Verify BrotliDecoder.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Validate_System_Void_P0() { BrotliDecoder.Validate(null!); }
    // Purpose: Verify BrotliDecoder.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Value_System_Void() { BrotliDecoder.Value; }
    // Purpose: Verify BrotliDecoder.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Value_System_Void_P0() { BrotliDecoder.Value = null!; }
    // Purpose: Verify BrotliDecoder.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Count_System_Void() { BrotliDecoder.Count; }
    // Purpose: Verify BrotliDecoder.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Count_System_Void_P0() { BrotliDecoder.Count = null!; }
    // Purpose: Verify BrotliDecoder.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_IsEmpty_System_Void() { BrotliDecoder.IsEmpty; }
    // Purpose: Verify BrotliDecoder.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_IsEmpty_System_Void_P0() { BrotliDecoder.IsEmpty = null!; }
    // Purpose: Verify BrotliDecoder.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Key_System_Void() { BrotliDecoder.Key; }
    // Purpose: Verify BrotliDecoder.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Key_System_Void_P0() { BrotliDecoder.Key = null!; }}
