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

// Auto-generated patch-side skeletons for FamilySystem.threading.tasks.parallelLoopResultPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.threading.tasks.parallelLoopResultPatch
{
    // Purpose: Verify ParallelLoopResult.ParallelLoopResult: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::ParallelLoopResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_ParallelLoopResult_Void_System_Object() { _ = ParallelLoopResult.ParallelLoopResult:(42); }
    // Purpose: Verify ParallelLoopResult.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Property_System_Void() { ParallelLoopResult.Property; }
    // Purpose: Verify ParallelLoopResult.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method1_System_Void_P0() { ParallelLoopResult.Method1(null!); }
    // Purpose: Verify ParallelLoopResult.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method2_System_Void_P0_P1() { ParallelLoopResult.Method2(null!, null!); }
    // Purpose: Verify ParallelLoopResult.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Validate_System_Void_P0() { ParallelLoopResult.Validate(null!); }
    // Purpose: Verify ParallelLoopResult.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Value_System_Void() { ParallelLoopResult.Value; }
    // Purpose: Verify ParallelLoopResult.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Value_System_Void_P0() { ParallelLoopResult.Value = null!; }
    // Purpose: Verify ParallelLoopResult.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Count_System_Void() { ParallelLoopResult.Count; }
    // Purpose: Verify ParallelLoopResult.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Count_System_Void_P0() { ParallelLoopResult.Count = null!; }
    // Purpose: Verify ParallelLoopResult.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_IsEmpty_System_Void() { ParallelLoopResult.IsEmpty; }
    // Purpose: Verify ParallelLoopResult.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_IsEmpty_System_Void_P0() { ParallelLoopResult.IsEmpty = null!; }
    // Purpose: Verify ParallelLoopResult.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Key_System_Void() { ParallelLoopResult.Key; }
    // Purpose: Verify ParallelLoopResult.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Key_System_Void_P0() { ParallelLoopResult.Key = null!; }}
