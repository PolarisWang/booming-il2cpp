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

// Auto-generated hot-update skeletons for FamilySystem.collections.immutableImmutableDictionaryHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.collections.immutableImmutableDictionaryHotUpdate
{
    // Purpose: Verify ImmutableDictionary.ImmutableDictionary: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::ImmutableDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_ImmutableDictionary_Void_System_Object() { _ = ImmutableDictionary.ImmutableDictionary:(42); }
    // Purpose: Verify ImmutableDictionary.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Property_System_Void() { ImmutableDictionary.Property; }
    // Purpose: Verify ImmutableDictionary.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method1_System_Void_P0() { ImmutableDictionary.Method1(null!); }
    // Purpose: Verify ImmutableDictionary.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method2_System_Void_P0_P1() { ImmutableDictionary.Method2(null!, null!); }
    // Purpose: Verify ImmutableDictionary.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Validate_System_Void_P0() { ImmutableDictionary.Validate(null!); }
    // Purpose: Verify ImmutableDictionary.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Value_System_Void() { ImmutableDictionary.Value; }
    // Purpose: Verify ImmutableDictionary.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Value_System_Void_P0() { ImmutableDictionary.Value = null!; }
    // Purpose: Verify ImmutableDictionary.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Count_System_Void() { ImmutableDictionary.Count; }
    // Purpose: Verify ImmutableDictionary.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Count_System_Void_P0() { ImmutableDictionary.Count = null!; }
    // Purpose: Verify ImmutableDictionary.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_IsEmpty_System_Void() { ImmutableDictionary.IsEmpty; }
    // Purpose: Verify ImmutableDictionary.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_IsEmpty_System_Void_P0() { ImmutableDictionary.IsEmpty = null!; }
    // Purpose: Verify ImmutableDictionary.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Key_System_Void() { ImmutableDictionary.Key; }
    // Purpose: Verify ImmutableDictionary.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Key_System_Void_P0() { ImmutableDictionary.Key = null!; }}
