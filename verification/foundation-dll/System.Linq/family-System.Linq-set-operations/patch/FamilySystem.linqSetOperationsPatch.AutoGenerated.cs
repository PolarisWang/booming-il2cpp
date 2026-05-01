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

// Auto-generated patch-side skeletons for FamilySystem.linqSetOperationsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.linqSetOperationsPatch
{
    // Purpose: Verify Enumerable.Enumerable: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::Enumerable::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_Enumerable_Void_System_Object() { _ = Enumerable.Enumerable:(42); }
    // Purpose: Verify Enumerable.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_get_Property_System_Void() { Enumerable.Property; }
    // Purpose: Verify Enumerable.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_Method1_System_Void_P0() { Enumerable.Method1(null!); }
    // Purpose: Verify Enumerable.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_Method2_System_Void_P0_P1() { Enumerable.Method2(null!, null!); }
    // Purpose: Verify Enumerable.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_Validate_System_Void_P0() { Enumerable.Validate(null!); }
    // Purpose: Verify IGrouping.IGrouping: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::IGrouping::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_IGrouping_Void_System_Object() { _ = IGrouping.IGrouping:(42); }
    // Purpose: Verify IGrouping.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_get_Property_System_Void() { IGrouping.Property; }
    // Purpose: Verify IGrouping.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_Method1_System_Void_P0() { IGrouping.Method1(null!); }
    // Purpose: Verify IGrouping.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_Method2_System_Void_P0_P1() { IGrouping.Method2(null!, null!); }
    // Purpose: Verify IGrouping.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_Validate_System_Void_P0() { IGrouping.Validate(null!); }
    // Purpose: Verify ILookup.ILookup: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::ILookup::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_ILookup_Void_System_Object() { _ = ILookup.ILookup:(42); }
    // Purpose: Verify ILookup.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_get_Property_System_Void() { ILookup.Property; }
    // Purpose: Verify ILookup.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_Method1_System_Void_P0() { ILookup.Method1(null!); }
    // Purpose: Verify ILookup.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_Method2_System_Void_P0_P1() { ILookup.Method2(null!, null!); }
    // Purpose: Verify ILookup.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_Validate_System_Void_P0() { ILookup.Validate(null!); }
    // Purpose: Verify IOrderedEnumerable.IOrderedEnumerable: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::IOrderedEnumerable::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_IOrderedEnumerable_Void_System_Object() { _ = IOrderedEnumerable.IOrderedEnumerable:(42); }
    // Purpose: Verify IOrderedEnumerable.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_get_Property_System_Void() { IOrderedEnumerable.Property; }
    // Purpose: Verify IOrderedEnumerable.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_Method1_System_Void_P0() { IOrderedEnumerable.Method1(null!); }
    // Purpose: Verify IOrderedEnumerable.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_Method2_System_Void_P0_P1() { IOrderedEnumerable.Method2(null!, null!); }
    // Purpose: Verify IOrderedEnumerable.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_Validate_System_Void_P0() { IOrderedEnumerable.Validate(null!); }
    // Purpose: Verify Lookup.Lookup: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::Lookup::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_Lookup_Void_System_Object() { _ = Lookup.Lookup:(42); }
    // Purpose: Verify Lookup.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_get_Property_System_Void() { Lookup.Property; }
    // Purpose: Verify Lookup.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_Method1_System_Void_P0() { Lookup.Method1(null!); }
    // Purpose: Verify Lookup.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_Method2_System_Void_P0_P1() { Lookup.Method2(null!, null!); }
    // Purpose: Verify Lookup.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_Validate_System_Void_P0() { Lookup.Validate(null!); }
    // Purpose: Verify Enumerable.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_get_Value_System_Void() { Enumerable.Value; }
    // Purpose: Verify Enumerable.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_set_Value_System_Void_P0() { Enumerable.Value = null!; }
    // Purpose: Verify Enumerable.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_get_Count_System_Void() { Enumerable.Count; }
    // Purpose: Verify Enumerable.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_set_Count_System_Void_P0() { Enumerable.Count = null!; }
    // Purpose: Verify Enumerable.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_get_IsEmpty_System_Void() { Enumerable.IsEmpty; }
    // Purpose: Verify Enumerable.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_set_IsEmpty_System_Void_P0() { Enumerable.IsEmpty = null!; }
    // Purpose: Verify Enumerable.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_get_Key_System_Void() { Enumerable.Key; }
    // Purpose: Verify Enumerable.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Enumerable::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Enumerable_set_Key_System_Void_P0() { Enumerable.Key = null!; }
    // Purpose: Verify IGrouping.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_get_Value_System_Void() { IGrouping.Value; }
    // Purpose: Verify IGrouping.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_set_Value_System_Void_P0() { IGrouping.Value = null!; }
    // Purpose: Verify IGrouping.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_get_Count_System_Void() { IGrouping.Count; }
    // Purpose: Verify IGrouping.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_set_Count_System_Void_P0() { IGrouping.Count = null!; }
    // Purpose: Verify IGrouping.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_get_IsEmpty_System_Void() { IGrouping.IsEmpty; }
    // Purpose: Verify IGrouping.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_set_IsEmpty_System_Void_P0() { IGrouping.IsEmpty = null!; }
    // Purpose: Verify IGrouping.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_get_Key_System_Void() { IGrouping.Key; }
    // Purpose: Verify IGrouping.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IGrouping::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IGrouping_set_Key_System_Void_P0() { IGrouping.Key = null!; }
    // Purpose: Verify ILookup.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_get_Value_System_Void() { ILookup.Value; }
    // Purpose: Verify ILookup.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_set_Value_System_Void_P0() { ILookup.Value = null!; }
    // Purpose: Verify ILookup.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_get_Count_System_Void() { ILookup.Count; }
    // Purpose: Verify ILookup.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_set_Count_System_Void_P0() { ILookup.Count = null!; }
    // Purpose: Verify ILookup.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_get_IsEmpty_System_Void() { ILookup.IsEmpty; }
    // Purpose: Verify ILookup.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_set_IsEmpty_System_Void_P0() { ILookup.IsEmpty = null!; }
    // Purpose: Verify ILookup.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_get_Key_System_Void() { ILookup.Key; }
    // Purpose: Verify ILookup.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.ILookup::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_ILookup_set_Key_System_Void_P0() { ILookup.Key = null!; }
    // Purpose: Verify IOrderedEnumerable.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_get_Value_System_Void() { IOrderedEnumerable.Value; }
    // Purpose: Verify IOrderedEnumerable.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_set_Value_System_Void_P0() { IOrderedEnumerable.Value = null!; }
    // Purpose: Verify IOrderedEnumerable.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_get_Count_System_Void() { IOrderedEnumerable.Count; }
    // Purpose: Verify IOrderedEnumerable.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_set_Count_System_Void_P0() { IOrderedEnumerable.Count = null!; }
    // Purpose: Verify IOrderedEnumerable.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_get_IsEmpty_System_Void() { IOrderedEnumerable.IsEmpty; }
    // Purpose: Verify IOrderedEnumerable.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_set_IsEmpty_System_Void_P0() { IOrderedEnumerable.IsEmpty = null!; }
    // Purpose: Verify IOrderedEnumerable.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_get_Key_System_Void() { IOrderedEnumerable.Key; }
    // Purpose: Verify IOrderedEnumerable.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_IOrderedEnumerable_set_Key_System_Void_P0() { IOrderedEnumerable.Key = null!; }
    // Purpose: Verify Lookup.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_get_Value_System_Void() { Lookup.Value; }
    // Purpose: Verify Lookup.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_set_Value_System_Void_P0() { Lookup.Value = null!; }
    // Purpose: Verify Lookup.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_get_Count_System_Void() { Lookup.Count; }
    // Purpose: Verify Lookup.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_set_Count_System_Void_P0() { Lookup.Count = null!; }
    // Purpose: Verify Lookup.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_get_IsEmpty_System_Void() { Lookup.IsEmpty; }
    // Purpose: Verify Lookup.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_set_IsEmpty_System_Void_P0() { Lookup.IsEmpty = null!; }
    // Purpose: Verify Lookup.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_get_Key_System_Void() { Lookup.Key; }
    // Purpose: Verify Lookup.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Linq/System.Linq.Lookup::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Linq_System_Linq_Lookup_set_Key_System_Void_P0() { Lookup.Key = null!; }}
