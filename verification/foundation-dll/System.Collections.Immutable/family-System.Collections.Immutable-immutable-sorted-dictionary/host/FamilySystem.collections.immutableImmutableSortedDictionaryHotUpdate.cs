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

// Hot-update stubs for family/System.Collections.Immutable/immutable-sorted-dictionary
public static class FamilySystem.collections.immutableImmutableSortedDictionaryHotUpdate
{
    // Verify ImmutableSortedDictionary.ImmutableSortedDictionary: after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::ImmutableSortedDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_ImmutableSortedDictionary_Void_Syste() { _ = ImmutableSortedDictionary.ImmutableSortedDictionary:(42); }
    // Verify ImmutableSortedDictionary.get_Property after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Property_System_Void() { ImmutableSortedDictionary.Property; }
    // Verify ImmutableSortedDictionary.Method1 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method1_System_Void_P0() { ImmutableSortedDictionary.Method1(null!); }
    // Verify ImmutableSortedDictionary.Method2 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method2_System_Void_P0_P1() { ImmutableSortedDictionary.Method2(null!, null!); }
    // Verify ImmutableSortedDictionary.Validate after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Validate_System_Void_P0() { ImmutableSortedDictionary.Validate(null!); }
    // Verify ImmutableSortedDictionary.get_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Value_System_Void() { ImmutableSortedDictionary.Value; }
    // Verify ImmutableSortedDictionary.set_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Value_System_Void_P0() { ImmutableSortedDictionary.Value = null!; }
    // Verify ImmutableSortedDictionary.get_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Count_System_Void() { ImmutableSortedDictionary.Count; }
    // Verify ImmutableSortedDictionary.set_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Count_System_Void_P0() { ImmutableSortedDictionary.Count = null!; }
    // Verify ImmutableSortedDictionary.get_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_IsEmpty_System_Void() { ImmutableSortedDictionary.IsEmpty; }
    // Verify ImmutableSortedDictionary.set_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_IsEmpty_System_Void_P0() { ImmutableSortedDictionary.IsEmpty = null!; }
    // Verify ImmutableSortedDictionary.get_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Key_System_Void() { ImmutableSortedDictionary.Key; }
    // Verify ImmutableSortedDictionary.set_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Key_System_Void_P0() { ImmutableSortedDictionary.Key = null!; }}
