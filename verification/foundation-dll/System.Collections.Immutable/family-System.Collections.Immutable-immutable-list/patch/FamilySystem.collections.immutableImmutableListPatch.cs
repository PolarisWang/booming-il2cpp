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

// Hot-update stubs for family/System.Collections.Immutable/immutable-list
public static class FamilySystem.collections.immutableImmutableListPatch
{
    // Verify ImmutableList.ImmutableList: after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::ImmutableList::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_ImmutableList_Void_System_Object() { _ = ImmutableList.ImmutableList:(42); }
    // Verify ImmutableList.get_Property after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Property_System_Void() { ImmutableList.Property; }
    // Verify ImmutableList.Method1 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Method1_System_Void_P0() { ImmutableList.Method1(null!); }
    // Verify ImmutableList.Method2 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Method2_System_Void_P0_P1() { ImmutableList.Method2(null!, null!); }
    // Verify ImmutableList.Validate after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Validate_System_Void_P0() { ImmutableList.Validate(null!); }
    // Verify ImmutableList.get_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Value_System_Void() { ImmutableList.Value; }
    // Verify ImmutableList.set_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Value_System_Void_P0() { ImmutableList.Value = null!; }
    // Verify ImmutableList.get_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Count_System_Void() { ImmutableList.Count; }
    // Verify ImmutableList.set_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Count_System_Void_P0() { ImmutableList.Count = null!; }
    // Verify ImmutableList.get_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_IsEmpty_System_Void() { ImmutableList.IsEmpty; }
    // Verify ImmutableList.set_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_IsEmpty_System_Void_P0() { ImmutableList.IsEmpty = null!; }
    // Verify ImmutableList.get_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Key_System_Void() { ImmutableList.Key; }
    // Verify ImmutableList.set_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Key_System_Void_P0() { ImmutableList.Key = null!; }}
