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

// Hot-update stubs for family/System.Collections.Immutable/immutable-array
public static class FamilySystem.collections.immutableImmutableArrayHotUpdate
{
    // Verify ImmutableArray.ImmutableArray: after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::ImmutableArray::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_ImmutableArray_Void_System_Object() { _ = ImmutableArray.ImmutableArray:(42); }
    // Verify ImmutableArray.get_Property after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Property_System_Void() { ImmutableArray.Property; }
    // Verify ImmutableArray.Method1 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Method1_System_Void_P0() { ImmutableArray.Method1(null!); }
    // Verify ImmutableArray.Method2 after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Method2_System_Void_P0_P1() { ImmutableArray.Method2(null!, null!); }
    // Verify ImmutableArray.Validate after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Validate_System_Void_P0() { ImmutableArray.Validate(null!); }
    // Verify ImmutableArray.get_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Value_System_Void() { ImmutableArray.Value; }
    // Verify ImmutableArray.set_Value after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Value_System_Void_P0() { ImmutableArray.Value = null!; }
    // Verify ImmutableArray.get_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Count_System_Void() { ImmutableArray.Count; }
    // Verify ImmutableArray.set_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Count_System_Void_P0() { ImmutableArray.Count = null!; }
    // Verify ImmutableArray.get_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_IsEmpty_System_Void() { ImmutableArray.IsEmpty; }
    // Verify ImmutableArray.set_IsEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_IsEmpty_System_Void_P0() { ImmutableArray.IsEmpty = null!; }
    // Verify ImmutableArray.get_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Key_System_Void() { ImmutableArray.Key; }
    // Verify ImmutableArray.set_Key after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Key_System_Void_P0() { ImmutableArray.Key = null!; }}
