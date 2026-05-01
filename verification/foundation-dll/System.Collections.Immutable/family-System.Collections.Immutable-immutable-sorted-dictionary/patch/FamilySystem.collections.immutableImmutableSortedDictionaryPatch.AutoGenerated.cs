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

// Auto-generated patch-side skeletons for FamilySystem.collections.immutableImmutableSortedDictionaryPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.collections.immutableImmutableSortedDictionaryPatch
{
    // Purpose: Verify ImmutableSortedDictionary.ImmutableSortedDictionary: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::ImmutableSortedDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_ImmutableSortedDictionary_Void_Syste() { _ = ImmutableSortedDictionary.ImmutableSortedDictionary:(42); }
    // Purpose: Verify ImmutableSortedDictionary.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Property_System_Void() { ImmutableSortedDictionary.Property; }
    // Purpose: Verify ImmutableSortedDictionary.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method1_System_Void_P0() { ImmutableSortedDictionary.Method1(null!); }
    // Purpose: Verify ImmutableSortedDictionary.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method2_System_Void_P0_P1() { ImmutableSortedDictionary.Method2(null!, null!); }
    // Purpose: Verify ImmutableSortedDictionary.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Validate_System_Void_P0() { ImmutableSortedDictionary.Validate(null!); }
    // Purpose: Verify ImmutableSortedDictionary.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Value_System_Void() { ImmutableSortedDictionary.Value; }
    // Purpose: Verify ImmutableSortedDictionary.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Value_System_Void_P0() { ImmutableSortedDictionary.Value = null!; }
    // Purpose: Verify ImmutableSortedDictionary.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Count_System_Void() { ImmutableSortedDictionary.Count; }
    // Purpose: Verify ImmutableSortedDictionary.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Count_System_Void_P0() { ImmutableSortedDictionary.Count = null!; }
    // Purpose: Verify ImmutableSortedDictionary.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_IsEmpty_System_Void() { ImmutableSortedDictionary.IsEmpty; }
    // Purpose: Verify ImmutableSortedDictionary.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_IsEmpty_System_Void_P0() { ImmutableSortedDictionary.IsEmpty = null!; }
    // Purpose: Verify ImmutableSortedDictionary.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Key_System_Void() { ImmutableSortedDictionary.Key; }
    // Purpose: Verify ImmutableSortedDictionary.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Key_System_Void_P0() { ImmutableSortedDictionary.Key = null!; }}
