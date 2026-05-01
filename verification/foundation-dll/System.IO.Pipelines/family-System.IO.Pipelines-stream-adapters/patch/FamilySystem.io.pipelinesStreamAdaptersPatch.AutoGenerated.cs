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

// Auto-generated patch-side skeletons for FamilySystem.io.pipelinesStreamAdaptersPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.pipelinesStreamAdaptersPatch
{
    // Purpose: Verify StreamPipeExtensions.StreamPipeExtensions: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::StreamPipeExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_StreamPipeExtensions_Void_System_Object() { _ = StreamPipeExtensions.StreamPipeExtensions:(42); }
    // Purpose: Verify StreamPipeExtensions.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Property_System_Void() { StreamPipeExtensions.Property; }
    // Purpose: Verify StreamPipeExtensions.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method1_System_Void_P0() { StreamPipeExtensions.Method1(null!); }
    // Purpose: Verify StreamPipeExtensions.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method2_System_Void_P0_P1() { StreamPipeExtensions.Method2(null!, null!); }
    // Purpose: Verify StreamPipeExtensions.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Validate_System_Void_P0() { StreamPipeExtensions.Validate(null!); }
    // Purpose: Verify StreamPipeExtensions.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Value_System_Void() { StreamPipeExtensions.Value; }
    // Purpose: Verify StreamPipeExtensions.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Value_System_Void_P0() { StreamPipeExtensions.Value = null!; }
    // Purpose: Verify StreamPipeExtensions.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Count_System_Void() { StreamPipeExtensions.Count; }
    // Purpose: Verify StreamPipeExtensions.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Count_System_Void_P0() { StreamPipeExtensions.Count = null!; }
    // Purpose: Verify StreamPipeExtensions.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_IsEmpty_System_Void() { StreamPipeExtensions.IsEmpty; }
    // Purpose: Verify StreamPipeExtensions.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_IsEmpty_System_Void_P0() { StreamPipeExtensions.IsEmpty = null!; }
    // Purpose: Verify StreamPipeExtensions.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Key_System_Void() { StreamPipeExtensions.Key; }
    // Purpose: Verify StreamPipeExtensions.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Key_System_Void_P0() { StreamPipeExtensions.Key = null!; }}
