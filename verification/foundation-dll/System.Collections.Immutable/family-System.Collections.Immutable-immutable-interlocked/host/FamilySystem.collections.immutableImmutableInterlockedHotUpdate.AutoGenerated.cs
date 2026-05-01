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

// Auto-generated hot-update skeletons for FamilySystem.collections.immutableImmutableInterlockedHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.collections.immutableImmutableInterlockedHotUpdate
{
    // Purpose: Verify ImmutableInterlocked.ImmutableInterlocked: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::ImmutableInterlocked::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_ImmutableInterlocked_Void_System_Object() { _ = ImmutableInterlocked.ImmutableInterlocked:(42); }
    // Purpose: Verify ImmutableInterlocked.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Property_System_Void() { ImmutableInterlocked.Property; }
    // Purpose: Verify ImmutableInterlocked.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method1_System_Void_P0() { ImmutableInterlocked.Method1(null!); }
    // Purpose: Verify ImmutableInterlocked.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method2_System_Void_P0_P1() { ImmutableInterlocked.Method2(null!, null!); }
    // Purpose: Verify ImmutableInterlocked.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Validate_System_Void_P0() { ImmutableInterlocked.Validate(null!); }
    // Purpose: Verify ImmutableInterlocked.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Value_System_Void() { ImmutableInterlocked.Value; }
    // Purpose: Verify ImmutableInterlocked.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Value_System_Void_P0() { ImmutableInterlocked.Value = null!; }
    // Purpose: Verify ImmutableInterlocked.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Count_System_Void() { ImmutableInterlocked.Count; }
    // Purpose: Verify ImmutableInterlocked.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Count_System_Void_P0() { ImmutableInterlocked.Count = null!; }
    // Purpose: Verify ImmutableInterlocked.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_IsEmpty_System_Void() { ImmutableInterlocked.IsEmpty; }
    // Purpose: Verify ImmutableInterlocked.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_IsEmpty_System_Void_P0() { ImmutableInterlocked.IsEmpty = null!; }
    // Purpose: Verify ImmutableInterlocked.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Key_System_Void() { ImmutableInterlocked.Key; }
    // Purpose: Verify ImmutableInterlocked.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Key_System_Void_P0() { ImmutableInterlocked.Key = null!; }}
