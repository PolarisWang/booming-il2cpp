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

// Auto-generated patch-side skeletons for FamilySystem.collections.immutableFrozenCollectionsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.collections.immutableFrozenCollectionsPatch
{
    // Purpose: Verify FrozenDictionary.FrozenDictionary: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::FrozenDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object() { _ = FrozenDictionary.FrozenDictionary:(42); }
    // Purpose: Verify FrozenDictionary.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void() { FrozenDictionary.Property; }
    // Purpose: Verify FrozenDictionary.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0() { FrozenDictionary.Method1(null!); }
    // Purpose: Verify FrozenDictionary.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1() { FrozenDictionary.Method2(null!, null!); }
    // Purpose: Verify FrozenDictionary.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0() { FrozenDictionary.Validate(null!); }
    // Purpose: Verify FrozenSet.FrozenSet: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::FrozenSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object() { _ = FrozenSet.FrozenSet:(42); }
    // Purpose: Verify FrozenSet.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void() { FrozenSet.Property; }
    // Purpose: Verify FrozenSet.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0() { FrozenSet.Method1(null!); }
    // Purpose: Verify FrozenSet.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1() { FrozenSet.Method2(null!, null!); }
    // Purpose: Verify FrozenSet.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0() { FrozenSet.Validate(null!); }
    // Purpose: Verify FrozenDictionary.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void() { FrozenDictionary.Value; }
    // Purpose: Verify FrozenDictionary.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0() { FrozenDictionary.Value = null!; }
    // Purpose: Verify FrozenDictionary.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void() { FrozenDictionary.Count; }
    // Purpose: Verify FrozenDictionary.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0() { FrozenDictionary.Count = null!; }
    // Purpose: Verify FrozenDictionary.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void() { FrozenDictionary.IsEmpty; }
    // Purpose: Verify FrozenDictionary.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0() { FrozenDictionary.IsEmpty = null!; }
    // Purpose: Verify FrozenDictionary.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void() { FrozenDictionary.Key; }
    // Purpose: Verify FrozenDictionary.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0() { FrozenDictionary.Key = null!; }
    // Purpose: Verify FrozenSet.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void() { FrozenSet.Value; }
    // Purpose: Verify FrozenSet.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0() { FrozenSet.Value = null!; }
    // Purpose: Verify FrozenSet.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void() { FrozenSet.Count; }
    // Purpose: Verify FrozenSet.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0() { FrozenSet.Count = null!; }
    // Purpose: Verify FrozenSet.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void() { FrozenSet.IsEmpty; }
    // Purpose: Verify FrozenSet.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0() { FrozenSet.IsEmpty = null!; }
    // Purpose: Verify FrozenSet.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void() { FrozenSet.Key; }
    // Purpose: Verify FrozenSet.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0() { FrozenSet.Key = null!; }}
