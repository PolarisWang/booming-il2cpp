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

// Auto-generated patch-side skeletons for GenericNullableValuePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class GenericNullableValuePatch
{
    // Purpose: Verify Nullable.get_HasValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }
    // Purpose: Verify Nullable.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_get_Value_T() { }
    // Purpose: Verify Nullable.GetValueOrDefault executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }
    // Purpose: Verify Nullable.GetValueOrDefault executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }
    // Purpose: Verify Nullable.Equals executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }
    // Purpose: Verify Nullable.GetHashCode executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }
    // Purpose: Verify Nullable.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }
    // Purpose: Verify ValueType.Equals executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }
    // Purpose: Verify ValueType.GetHashCode executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }
    // Purpose: Verify ValueType.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }}
