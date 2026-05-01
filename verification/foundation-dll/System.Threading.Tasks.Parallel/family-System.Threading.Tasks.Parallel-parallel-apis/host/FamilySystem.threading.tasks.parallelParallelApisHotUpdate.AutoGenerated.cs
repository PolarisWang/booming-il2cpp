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

// Auto-generated hot-update skeletons for FamilySystem.threading.tasks.parallelParallelApisHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.threading.tasks.parallelParallelApisHotUpdate
{
    // Purpose: Verify Parallel.Parallel: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Parallel::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Parallel_Void_System_Object() { _ = Parallel.Parallel:(42); }
    // Purpose: Verify Parallel.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Property_System_Void() { Parallel.Property; }
    // Purpose: Verify Parallel.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Method1_System_Void_P0() { Parallel.Method1(null!); }
    // Purpose: Verify Parallel.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Method2_System_Void_P0_P1() { Parallel.Method2(null!, null!); }
    // Purpose: Verify Parallel.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Validate_System_Void_P0() { Parallel.Validate(null!); }
    // Purpose: Verify Parallel.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Value_System_Void() { Parallel.Value; }
    // Purpose: Verify Parallel.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Value_System_Void_P0() { Parallel.Value = null!; }
    // Purpose: Verify Parallel.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Count_System_Void() { Parallel.Count; }
    // Purpose: Verify Parallel.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Count_System_Void_P0() { Parallel.Count = null!; }
    // Purpose: Verify Parallel.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_IsEmpty_System_Void() { Parallel.IsEmpty; }
    // Purpose: Verify Parallel.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_IsEmpty_System_Void_P0() { Parallel.IsEmpty = null!; }
    // Purpose: Verify Parallel.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Key_System_Void() { Parallel.Key; }
    // Purpose: Verify Parallel.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Key_System_Void_P0() { Parallel.Key = null!; }}
