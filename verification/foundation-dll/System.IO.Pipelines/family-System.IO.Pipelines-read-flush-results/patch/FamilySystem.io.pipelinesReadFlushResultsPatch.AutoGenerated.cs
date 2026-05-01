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

// Auto-generated patch-side skeletons for FamilySystem.io.pipelinesReadFlushResultsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.pipelinesReadFlushResultsPatch
{
    // Purpose: Verify FlushResult.FlushResult: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::FlushResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_FlushResult_Void_System_Object() { _ = FlushResult.FlushResult:(42); }
    // Purpose: Verify FlushResult.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Property_System_Void() { FlushResult.Property; }
    // Purpose: Verify FlushResult.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method1_System_Void_P0() { FlushResult.Method1(null!); }
    // Purpose: Verify FlushResult.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method2_System_Void_P0_P1() { FlushResult.Method2(null!, null!); }
    // Purpose: Verify FlushResult.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Validate_System_Void_P0() { FlushResult.Validate(null!); }
    // Purpose: Verify ReadResult.ReadResult: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::ReadResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_ReadResult_Void_System_Object() { _ = ReadResult.ReadResult:(42); }
    // Purpose: Verify ReadResult.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Property_System_Void() { ReadResult.Property; }
    // Purpose: Verify ReadResult.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method1_System_Void_P0() { ReadResult.Method1(null!); }
    // Purpose: Verify ReadResult.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method2_System_Void_P0_P1() { ReadResult.Method2(null!, null!); }
    // Purpose: Verify ReadResult.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Validate_System_Void_P0() { ReadResult.Validate(null!); }
    // Purpose: Verify FlushResult.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Value_System_Void() { FlushResult.Value; }
    // Purpose: Verify FlushResult.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Value_System_Void_P0() { FlushResult.Value = null!; }
    // Purpose: Verify FlushResult.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Count_System_Void() { FlushResult.Count; }
    // Purpose: Verify FlushResult.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Count_System_Void_P0() { FlushResult.Count = null!; }
    // Purpose: Verify FlushResult.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_IsEmpty_System_Void() { FlushResult.IsEmpty; }
    // Purpose: Verify FlushResult.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_IsEmpty_System_Void_P0() { FlushResult.IsEmpty = null!; }
    // Purpose: Verify FlushResult.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Key_System_Void() { FlushResult.Key; }
    // Purpose: Verify FlushResult.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Key_System_Void_P0() { FlushResult.Key = null!; }
    // Purpose: Verify ReadResult.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Value_System_Void() { ReadResult.Value; }
    // Purpose: Verify ReadResult.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Value_System_Void_P0() { ReadResult.Value = null!; }
    // Purpose: Verify ReadResult.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Count_System_Void() { ReadResult.Count; }
    // Purpose: Verify ReadResult.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Count_System_Void_P0() { ReadResult.Count = null!; }
    // Purpose: Verify ReadResult.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_IsEmpty_System_Void() { ReadResult.IsEmpty; }
    // Purpose: Verify ReadResult.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_IsEmpty_System_Void_P0() { ReadResult.IsEmpty = null!; }
    // Purpose: Verify ReadResult.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Key_System_Void() { ReadResult.Key; }
    // Purpose: Verify ReadResult.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Key_System_Void_P0() { ReadResult.Key = null!; }}
