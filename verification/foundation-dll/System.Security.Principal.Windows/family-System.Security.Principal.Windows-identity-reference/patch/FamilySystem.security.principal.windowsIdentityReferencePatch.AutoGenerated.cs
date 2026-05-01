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

// Auto-generated patch-side skeletons for FamilySystem.security.principal.windowsIdentityReferencePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.security.principal.windowsIdentityReferencePatch
{
    // Purpose: Verify IdentityReference.IdentityReference: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IdentityReference::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_IdentityReference_Void_System_Object() { _ = IdentityReference.IdentityReference:(42); }
    // Purpose: Verify IdentityReference.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Property_System_Void() { IdentityReference.Property; }
    // Purpose: Verify IdentityReference.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method1_System_Void_P0() { IdentityReference.Method1(null!); }
    // Purpose: Verify IdentityReference.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method2_System_Void_P0_P1() { IdentityReference.Method2(null!, null!); }
    // Purpose: Verify IdentityReference.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Validate_System_Void_P0() { IdentityReference.Validate(null!); }
    // Purpose: Verify IdentityReferenceCollection.IdentityReferenceCollection: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::IdentityReferenceCollection::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_IdentityReferenceCollection_Void() { _ = IdentityReferenceCollection.IdentityReferenceCollection:(42); }
    // Purpose: Verify IdentityReferenceCollection.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Property_System_Void() { IdentityReferenceCollection.Property; }
    // Purpose: Verify IdentityReferenceCollection.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method1_System_Void_P0() { IdentityReferenceCollection.Method1(null!); }
    // Purpose: Verify IdentityReferenceCollection.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method2_System_Void_P0_P1() { IdentityReferenceCollection.Method2(null!, null!); }
    // Purpose: Verify IdentityReferenceCollection.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Validate_System_Void_P0() { IdentityReferenceCollection.Validate(null!); }
    // Purpose: Verify IdentityReference.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Value_System_Void() { IdentityReference.Value; }
    // Purpose: Verify IdentityReference.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Value_System_Void_P0() { IdentityReference.Value = null!; }
    // Purpose: Verify IdentityReference.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Count_System_Void() { IdentityReference.Count; }
    // Purpose: Verify IdentityReference.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Count_System_Void_P0() { IdentityReference.Count = null!; }
    // Purpose: Verify IdentityReference.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_IsEmpty_System_Void() { IdentityReference.IsEmpty; }
    // Purpose: Verify IdentityReference.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_IsEmpty_System_Void_P0() { IdentityReference.IsEmpty = null!; }
    // Purpose: Verify IdentityReference.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Key_System_Void() { IdentityReference.Key; }
    // Purpose: Verify IdentityReference.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Key_System_Void_P0() { IdentityReference.Key = null!; }
    // Purpose: Verify IdentityReferenceCollection.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Value_System_Void() { IdentityReferenceCollection.Value; }
    // Purpose: Verify IdentityReferenceCollection.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Value_System_Void_P0() { IdentityReferenceCollection.Value = null!; }
    // Purpose: Verify IdentityReferenceCollection.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Count_System_Void() { IdentityReferenceCollection.Count; }
    // Purpose: Verify IdentityReferenceCollection.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Count_System_Void_P0() { IdentityReferenceCollection.Count = null!; }
    // Purpose: Verify IdentityReferenceCollection.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_IsEmpty_System_Void() { IdentityReferenceCollection.IsEmpty; }
    // Purpose: Verify IdentityReferenceCollection.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_IsEmpty_System_Void_P0() { IdentityReferenceCollection.IsEmpty = null!; }
    // Purpose: Verify IdentityReferenceCollection.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Key_System_Void() { IdentityReferenceCollection.Key; }
    // Purpose: Verify IdentityReferenceCollection.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Key_System_Void_P0() { IdentityReferenceCollection.Key = null!; }}
