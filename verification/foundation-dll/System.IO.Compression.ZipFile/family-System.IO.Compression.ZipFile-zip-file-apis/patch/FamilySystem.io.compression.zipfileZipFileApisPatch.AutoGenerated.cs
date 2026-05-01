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

// Auto-generated patch-side skeletons for FamilySystem.io.compression.zipfileZipFileApisPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.compression.zipfileZipFileApisPatch
{
    // Purpose: Verify ZipFile.ZipFile: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ZipFile::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_ZipFile_Void_System_Object() { _ = ZipFile.ZipFile:(42); }
    // Purpose: Verify ZipFile.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Property_System_Void() { ZipFile.Property; }
    // Purpose: Verify ZipFile.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Method1_System_Void_P0() { ZipFile.Method1(null!); }
    // Purpose: Verify ZipFile.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Method2_System_Void_P0_P1() { ZipFile.Method2(null!, null!); }
    // Purpose: Verify ZipFile.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Validate_System_Void_P0() { ZipFile.Validate(null!); }
    // Purpose: Verify ZipFile.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Value_System_Void() { ZipFile.Value; }
    // Purpose: Verify ZipFile.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Value_System_Void_P0() { ZipFile.Value = null!; }
    // Purpose: Verify ZipFile.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Count_System_Void() { ZipFile.Count; }
    // Purpose: Verify ZipFile.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Count_System_Void_P0() { ZipFile.Count = null!; }
    // Purpose: Verify ZipFile.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_IsEmpty_System_Void() { ZipFile.IsEmpty; }
    // Purpose: Verify ZipFile.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_IsEmpty_System_Void_P0() { ZipFile.IsEmpty = null!; }
    // Purpose: Verify ZipFile.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Key_System_Void() { ZipFile.Key; }
    // Purpose: Verify ZipFile.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Key_System_Void_P0() { ZipFile.Key = null!; }}
