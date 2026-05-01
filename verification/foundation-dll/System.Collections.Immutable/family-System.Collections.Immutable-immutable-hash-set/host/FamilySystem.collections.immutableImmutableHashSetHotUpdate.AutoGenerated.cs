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

// Auto-generated hot-update skeletons for FamilySystem.collections.immutableImmutableHashSetHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.collections.immutableImmutableHashSetHotUpdate
{
    // Purpose: Verify ImmutableHashSet.ImmutableHashSet: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::ImmutableHashSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_ImmutableHashSet_Void_System_Object() { _ = ImmutableHashSet.ImmutableHashSet:(42); }
    // Purpose: Verify ImmutableHashSet.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_get_Property_System_Void() { ImmutableHashSet.Property; }
    // Purpose: Verify ImmutableHashSet.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Method1_System_Void_P0() { ImmutableHashSet.Method1(null!); }
    // Purpose: Verify ImmutableHashSet.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Method2_System_Void_P0_P1() { ImmutableHashSet.Method2(null!, null!); }
    // Purpose: Verify ImmutableHashSet.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Validate_System_Void_P0() { ImmutableHashSet.Validate(null!); }
    // Purpose: Verify ImmutableHashSet.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_get_Value_System_Void() { ImmutableHashSet.Value; }
    // Purpose: Verify ImmutableHashSet.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_set_Value_System_Void_P0() { ImmutableHashSet.Value = null!; }
    // Purpose: Verify ImmutableHashSet.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_get_Count_System_Void() { ImmutableHashSet.Count; }
    // Purpose: Verify ImmutableHashSet.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_set_Count_System_Void_P0() { ImmutableHashSet.Count = null!; }
    // Purpose: Verify ImmutableHashSet.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_get_IsEmpty_System_Void() { ImmutableHashSet.IsEmpty; }
    // Purpose: Verify ImmutableHashSet.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_set_IsEmpty_System_Void_P0() { ImmutableHashSet.IsEmpty = null!; }
    // Purpose: Verify ImmutableHashSet.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_get_Key_System_Void() { ImmutableHashSet.Key; }
    // Purpose: Verify ImmutableHashSet.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_set_Key_System_Void_P0() { ImmutableHashSet.Key = null!; }}
