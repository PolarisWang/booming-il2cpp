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

// Auto-generated hot-update skeletons for FamilySystem.threading.tasks.parallelParallelOptionsHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.threading.tasks.parallelParallelOptionsHotUpdate
{
    // Purpose: Verify ParallelOptions.ParallelOptions: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::ParallelOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_ParallelOptions_Void_System_Object() { _ = ParallelOptions.ParallelOptions:(42); }
    // Purpose: Verify ParallelOptions.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Property_System_Void() { ParallelOptions.Property; }
    // Purpose: Verify ParallelOptions.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Method1_System_Void_P0() { ParallelOptions.Method1(null!); }
    // Purpose: Verify ParallelOptions.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Method2_System_Void_P0_P1() { ParallelOptions.Method2(null!, null!); }
    // Purpose: Verify ParallelOptions.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Validate_System_Void_P0() { ParallelOptions.Validate(null!); }
    // Purpose: Verify ParallelOptions.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Value_System_Void() { ParallelOptions.Value; }
    // Purpose: Verify ParallelOptions.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Value_System_Void_P0() { ParallelOptions.Value = null!; }
    // Purpose: Verify ParallelOptions.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Count_System_Void() { ParallelOptions.Count; }
    // Purpose: Verify ParallelOptions.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Count_System_Void_P0() { ParallelOptions.Count = null!; }
    // Purpose: Verify ParallelOptions.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_IsEmpty_System_Void() { ParallelOptions.IsEmpty; }
    // Purpose: Verify ParallelOptions.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_IsEmpty_System_Void_P0() { ParallelOptions.IsEmpty = null!; }
    // Purpose: Verify ParallelOptions.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Key_System_Void() { ParallelOptions.Key; }
    // Purpose: Verify ParallelOptions.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Key_System_Void_P0() { ParallelOptions.Key = null!; }}
