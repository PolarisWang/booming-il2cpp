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

// Auto-generated patch-side skeletons for FamilySystem.io.pipelinesPipeCorePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.io.pipelinesPipeCorePatch
{
    // Purpose: Verify IDuplexPipe.IDuplexPipe: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::IDuplexPipe::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_IDuplexPipe_Void_System_Object() { _ = IDuplexPipe.IDuplexPipe:(42); }
    // Purpose: Verify IDuplexPipe.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Property_System_Void() { IDuplexPipe.Property; }
    // Purpose: Verify IDuplexPipe.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method1_System_Void_P0() { IDuplexPipe.Method1(null!); }
    // Purpose: Verify IDuplexPipe.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method2_System_Void_P0_P1() { IDuplexPipe.Method2(null!, null!); }
    // Purpose: Verify IDuplexPipe.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Validate_System_Void_P0() { IDuplexPipe.Validate(null!); }
    // Purpose: Verify Pipe.Pipe: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Pipe::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_Pipe_Void_System_Object() { _ = Pipe.Pipe:(42); }
    // Purpose: Verify Pipe.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Property_System_Void() { Pipe.Property; }
    // Purpose: Verify Pipe.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_Method1_System_Void_P0() { Pipe.Method1(null!); }
    // Purpose: Verify Pipe.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_Method2_System_Void_P0_P1() { Pipe.Method2(null!, null!); }
    // Purpose: Verify Pipe.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_Validate_System_Void_P0() { Pipe.Validate(null!); }
    // Purpose: Verify IDuplexPipe.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Value_System_Void() { IDuplexPipe.Value; }
    // Purpose: Verify IDuplexPipe.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Value_System_Void_P0() { IDuplexPipe.Value = null!; }
    // Purpose: Verify IDuplexPipe.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Count_System_Void() { IDuplexPipe.Count; }
    // Purpose: Verify IDuplexPipe.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Count_System_Void_P0() { IDuplexPipe.Count = null!; }
    // Purpose: Verify IDuplexPipe.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_IsEmpty_System_Void() { IDuplexPipe.IsEmpty; }
    // Purpose: Verify IDuplexPipe.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_IsEmpty_System_Void_P0() { IDuplexPipe.IsEmpty = null!; }
    // Purpose: Verify IDuplexPipe.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Key_System_Void() { IDuplexPipe.Key; }
    // Purpose: Verify IDuplexPipe.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Key_System_Void_P0() { IDuplexPipe.Key = null!; }
    // Purpose: Verify Pipe.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Value_System_Void() { Pipe.Value; }
    // Purpose: Verify Pipe.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Value_System_Void_P0() { Pipe.Value = null!; }
    // Purpose: Verify Pipe.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Count_System_Void() { Pipe.Count; }
    // Purpose: Verify Pipe.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Count_System_Void_P0() { Pipe.Count = null!; }
    // Purpose: Verify Pipe.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_IsEmpty_System_Void() { Pipe.IsEmpty; }
    // Purpose: Verify Pipe.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_IsEmpty_System_Void_P0() { Pipe.IsEmpty = null!; }
    // Purpose: Verify Pipe.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Key_System_Void() { Pipe.Key; }
    // Purpose: Verify Pipe.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Key_System_Void_P0() { Pipe.Key = null!; }}
