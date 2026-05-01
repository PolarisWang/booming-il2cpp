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

// Auto-generated patch-side skeletons for FamilySystem.io.compression.zipfileZipFileExtensionsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.compression.zipfileZipFileExtensionsPatch
{
    // Purpose: Verify ZipFileExtensions.ZipFileExtensions: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ZipFileExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_ZipFileExtensions_Void_System_Object() { _ = ZipFileExtensions.ZipFileExtensions:(42); }
    // Purpose: Verify ZipFileExtensions.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Property_System_Void() { ZipFileExtensions.Property; }
    // Purpose: Verify ZipFileExtensions.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method1_System_Void_P0() { ZipFileExtensions.Method1(null!); }
    // Purpose: Verify ZipFileExtensions.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method2_System_Void_P0_P1() { ZipFileExtensions.Method2(null!, null!); }
    // Purpose: Verify ZipFileExtensions.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Validate_System_Void_P0() { ZipFileExtensions.Validate(null!); }
    // Purpose: Verify ZipFileExtensions.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Value_System_Void() { ZipFileExtensions.Value; }
    // Purpose: Verify ZipFileExtensions.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Value_System_Void_P0() { ZipFileExtensions.Value = null!; }
    // Purpose: Verify ZipFileExtensions.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Count_System_Void() { ZipFileExtensions.Count; }
    // Purpose: Verify ZipFileExtensions.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Count_System_Void_P0() { ZipFileExtensions.Count = null!; }
    // Purpose: Verify ZipFileExtensions.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_IsEmpty_System_Void() { ZipFileExtensions.IsEmpty; }
    // Purpose: Verify ZipFileExtensions.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_IsEmpty_System_Void_P0() { ZipFileExtensions.IsEmpty = null!; }
    // Purpose: Verify ZipFileExtensions.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Key_System_Void() { ZipFileExtensions.Key; }
    // Purpose: Verify ZipFileExtensions.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Key_System_Void_P0() { ZipFileExtensions.Key = null!; }}
