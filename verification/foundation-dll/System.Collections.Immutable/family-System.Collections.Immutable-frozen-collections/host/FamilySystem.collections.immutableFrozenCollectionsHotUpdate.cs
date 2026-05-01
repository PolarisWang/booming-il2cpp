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

// Hot-update stubs for family/System.Collections.Immutable/frozen-collections
public static class FamilySystem.collections.immutableFrozenCollectionsHotUpdate
{
    // Verify FrozenDictionary.FrozenDictionary: after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::FrozenDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object() { _ = FrozenDictionary.FrozenDictionary:(42); }
    // Verify FrozenDictionary.get_Property after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void() { FrozenDictionary.Property; }
    // Verify FrozenDictionary.Method1 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0() { FrozenDictionary.Method1(null!); }
    // Verify FrozenDictionary.Method2 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1() { FrozenDictionary.Method2(null!, null!); }
    // Verify FrozenDictionary.Validate after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0() { FrozenDictionary.Validate(null!); }
    // Verify FrozenSet.FrozenSet: after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::FrozenSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object() { _ = FrozenSet.FrozenSet:(42); }
    // Verify FrozenSet.get_Property after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void() { FrozenSet.Property; }
    // Verify FrozenSet.Method1 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0() { FrozenSet.Method1(null!); }
    // Verify FrozenSet.Method2 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1() { FrozenSet.Method2(null!, null!); }
    // Verify FrozenSet.Validate after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0() { FrozenSet.Validate(null!); }
    // Verify FrozenDictionary.get_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void() { FrozenDictionary.Value; }
    // Verify FrozenDictionary.set_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0() { FrozenDictionary.Value = null!; }
    // Verify FrozenDictionary.get_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void() { FrozenDictionary.Count; }
    // Verify FrozenDictionary.set_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0() { FrozenDictionary.Count = null!; }
    // Verify FrozenDictionary.get_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void() { FrozenDictionary.IsEmpty; }
    // Verify FrozenDictionary.set_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0() { FrozenDictionary.IsEmpty = null!; }
    // Verify FrozenDictionary.get_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void() { FrozenDictionary.Key; }
    // Verify FrozenDictionary.set_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0() { FrozenDictionary.Key = null!; }
    // Verify FrozenSet.get_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void() { FrozenSet.Value; }
    // Verify FrozenSet.set_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0() { FrozenSet.Value = null!; }
    // Verify FrozenSet.get_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void() { FrozenSet.Count; }
    // Verify FrozenSet.set_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0() { FrozenSet.Count = null!; }
    // Verify FrozenSet.get_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void() { FrozenSet.IsEmpty; }
    // Verify FrozenSet.set_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0() { FrozenSet.IsEmpty = null!; }
    // Verify FrozenSet.get_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void() { FrozenSet.Key; }
    // Verify FrozenSet.set_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0() { FrozenSet.Key = null!; }}
