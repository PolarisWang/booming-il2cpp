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

// Hot-update stubs for family/System.Collections.Immutable/extensions-marshal
public static class FamilySystem.collections.immutableExtensionsMarshalPatch
{
    // Verify ImmutableArrayExtensions.ImmutableArrayExtensions: after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::ImmutableArrayExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_ImmutableArrayExtensions_Void_System_Object() { _ = ImmutableArrayExtensions.ImmutableArrayExtensions:(42); }
    // Verify ImmutableArrayExtensions.get_Property after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Property_System_Void() { ImmutableArrayExtensions.Property; }
    // Verify ImmutableArrayExtensions.Method1 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method1_System_Void_P0() { ImmutableArrayExtensions.Method1(null!); }
    // Verify ImmutableArrayExtensions.Method2 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method2_System_Void_P0_P1() { ImmutableArrayExtensions.Method2(null!, null!); }
    // Verify ImmutableArrayExtensions.Validate after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Validate_System_Void_P0() { ImmutableArrayExtensions.Validate(null!); }
    // Verify ImmutableCollectionsMarshal.ImmutableCollectionsMarshal: after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::ImmutableCollectionsMarshal::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_ImmutableCollectionsMarshal_Void() { _ = ImmutableCollectionsMarshal.ImmutableCollectionsMarshal:(42); }
    // Verify ImmutableCollectionsMarshal.get_Property after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Property_System_Void() { ImmutableCollectionsMarshal.Property; }
    // Verify ImmutableCollectionsMarshal.Method1 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method1_System_Void_P0() { ImmutableCollectionsMarshal.Method1(null!); }
    // Verify ImmutableCollectionsMarshal.Method2 after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method2_System_Void_P0_P1() { ImmutableCollectionsMarshal.Method2(null!, null!); }
    // Verify ImmutableCollectionsMarshal.Validate after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Validate_System_Void_P0() { ImmutableCollectionsMarshal.Validate(null!); }
    // Verify ImmutableArrayExtensions.get_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Value_System_Void() { ImmutableArrayExtensions.Value; }
    // Verify ImmutableArrayExtensions.set_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Value_System_Void_P0() { ImmutableArrayExtensions.Value = null!; }
    // Verify ImmutableArrayExtensions.get_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Count_System_Void() { ImmutableArrayExtensions.Count; }
    // Verify ImmutableArrayExtensions.set_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Count_System_Void_P0() { ImmutableArrayExtensions.Count = null!; }
    // Verify ImmutableArrayExtensions.get_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_IsEmpty_System_Void() { ImmutableArrayExtensions.IsEmpty; }
    // Verify ImmutableArrayExtensions.set_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_IsEmpty_System_Void_P0() { ImmutableArrayExtensions.IsEmpty = null!; }
    // Verify ImmutableArrayExtensions.get_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Key_System_Void() { ImmutableArrayExtensions.Key; }
    // Verify ImmutableArrayExtensions.set_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Key_System_Void_P0() { ImmutableArrayExtensions.Key = null!; }
    // Verify ImmutableCollectionsMarshal.get_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Value_System_Void() { ImmutableCollectionsMarshal.Value; }
    // Verify ImmutableCollectionsMarshal.set_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Value_System_Void_P0() { ImmutableCollectionsMarshal.Value = null!; }
    // Verify ImmutableCollectionsMarshal.get_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Count_System_Void() { ImmutableCollectionsMarshal.Count; }
    // Verify ImmutableCollectionsMarshal.set_Count after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Count_System_Void_P0() { ImmutableCollectionsMarshal.Count = null!; }
    // Verify ImmutableCollectionsMarshal.get_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_IsEmpty_System_Void() { ImmutableCollectionsMarshal.IsEmpty; }
    // Verify ImmutableCollectionsMarshal.set_IsEmpty after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_IsEmpty_System_Void_P0() { ImmutableCollectionsMarshal.IsEmpty = null!; }
    // Verify ImmutableCollectionsMarshal.get_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Key_System_Void() { ImmutableCollectionsMarshal.Key; }
    // Verify ImmutableCollectionsMarshal.set_Key after hot-update (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Key_System_Void_P0() { ImmutableCollectionsMarshal.Key = null!; }}
