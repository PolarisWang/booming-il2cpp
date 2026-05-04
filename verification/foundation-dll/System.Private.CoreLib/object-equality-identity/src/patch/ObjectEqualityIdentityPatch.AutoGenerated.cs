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

// Auto-generated patch-side skeletons for ObjectEqualityIdentityPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ObjectEqualityIdentityPatch
{
    // Purpose: Verify Object.Equals executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object() { _ = new object().Equals(42); }
    // Purpose: Verify Object.Equals executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object() { _ = Object.Equals(42, 42); }
    // Purpose: Verify Object.ReferenceEquals executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object() { _ = Object.ReferenceEquals(42, 42); }
    // Purpose: Verify Object.GetHashCode executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_GetHashCode_System_Int32() { _ = new object().GetHashCode(); }
    // Purpose: Verify Object.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_ToString_System_String() { _ = new object().ToString(); }
    // Purpose: Verify Object.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_GetType_System_Type() { _ = new object().GetType(); }
    // Purpose: Verify Object.MemberwiseClone executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }}
