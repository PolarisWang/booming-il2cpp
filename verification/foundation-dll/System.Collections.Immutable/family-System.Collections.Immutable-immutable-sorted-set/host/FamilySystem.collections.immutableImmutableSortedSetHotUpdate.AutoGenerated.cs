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

// Auto-generated hot-update skeletons for FamilySystem.collections.immutableImmutableSortedSetHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.collections.immutableImmutableSortedSetHotUpdate
{
    // Purpose: Verify ImmutableSortedSet.ImmutableSortedSet: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::ImmutableSortedSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_ImmutableSortedSet_Void_System_Object() { _ = ImmutableSortedSet.ImmutableSortedSet:(42); }
    // Purpose: Verify ImmutableSortedSet.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_get_Property_System_Void() { ImmutableSortedSet.Property; }
    // Purpose: Verify ImmutableSortedSet.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Method1_System_Void_P0() { ImmutableSortedSet.Method1(null!); }
    // Purpose: Verify ImmutableSortedSet.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Method2_System_Void_P0_P1() { ImmutableSortedSet.Method2(null!, null!); }
    // Purpose: Verify ImmutableSortedSet.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_Validate_System_Void_P0() { ImmutableSortedSet.Validate(null!); }
    // Purpose: Verify ImmutableSortedSet.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_get_Value_System_Void() { ImmutableSortedSet.Value; }
    // Purpose: Verify ImmutableSortedSet.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_set_Value_System_Void_P0() { ImmutableSortedSet.Value = null!; }
    // Purpose: Verify ImmutableSortedSet.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_get_Count_System_Void() { ImmutableSortedSet.Count; }
    // Purpose: Verify ImmutableSortedSet.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_set_Count_System_Void_P0() { ImmutableSortedSet.Count = null!; }
    // Purpose: Verify ImmutableSortedSet.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_get_IsEmpty_System_Void() { ImmutableSortedSet.IsEmpty; }
    // Purpose: Verify ImmutableSortedSet.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_set_IsEmpty_System_Void_P0() { ImmutableSortedSet.IsEmpty = null!; }
    // Purpose: Verify ImmutableSortedSet.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_get_Key_System_Void() { ImmutableSortedSet.Key; }
    // Purpose: Verify ImmutableSortedSet.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedSet_set_Key_System_Void_P0() { ImmutableSortedSet.Key = null!; }}
