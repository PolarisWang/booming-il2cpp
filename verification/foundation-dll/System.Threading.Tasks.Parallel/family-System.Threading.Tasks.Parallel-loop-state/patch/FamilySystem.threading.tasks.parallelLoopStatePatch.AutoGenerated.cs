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

// Auto-generated patch-side skeletons for FamilySystem.threading.tasks.parallelLoopStatePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.threading.tasks.parallelLoopStatePatch
{
    // Purpose: Verify ParallelLoopState.ParallelLoopState: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::ParallelLoopState::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_ParallelLoopState_Void_System_Object() { _ = ParallelLoopState.ParallelLoopState:(42); }
    // Purpose: Verify ParallelLoopState.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Property_System_Void() { ParallelLoopState.Property; }
    // Purpose: Verify ParallelLoopState.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method1_System_Void_P0() { ParallelLoopState.Method1(null!); }
    // Purpose: Verify ParallelLoopState.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method2_System_Void_P0_P1() { ParallelLoopState.Method2(null!, null!); }
    // Purpose: Verify ParallelLoopState.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Validate_System_Void_P0() { ParallelLoopState.Validate(null!); }
    // Purpose: Verify ParallelLoopState.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Value_System_Void() { ParallelLoopState.Value; }
    // Purpose: Verify ParallelLoopState.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Value_System_Void_P0() { ParallelLoopState.Value = null!; }
    // Purpose: Verify ParallelLoopState.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Count_System_Void() { ParallelLoopState.Count; }
    // Purpose: Verify ParallelLoopState.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Count_System_Void_P0() { ParallelLoopState.Count = null!; }
    // Purpose: Verify ParallelLoopState.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_IsEmpty_System_Void() { ParallelLoopState.IsEmpty; }
    // Purpose: Verify ParallelLoopState.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_IsEmpty_System_Void_P0() { ParallelLoopState.IsEmpty = null!; }
    // Purpose: Verify ParallelLoopState.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Key_System_Void() { ParallelLoopState.Key; }
    // Purpose: Verify ParallelLoopState.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Key_System_Void_P0() { ParallelLoopState.Key = null!; }}
