using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/generic/nullable-value
public static class GenericNullableValuePatch
{
    // Verify Nullable.get_HasValue after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean() { _ = default(Nullable<int>).HasValue; }
    // Verify Nullable.get_Value after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_Value:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_get_Value_T() { _ = ((int?)42).Value; }
    // Verify Nullable.GetValueOrDefault after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T() { _ = default(Nullable<int>).GetValueOrDefault(); }
    // Verify Nullable.GetValueOrDefault after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T() { }
    // Verify Nullable.Equals after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object() { _ = default(Nullable<int>).Equals(42); }
    // Verify Nullable.GetHashCode after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32() { _ = default(Nullable<int>).GetHashCode(); }
    // Verify Nullable.ToString after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Nullable_1_ToString_System_String() { _ = default(Nullable<int>).ToString(); }
    // Verify ValueType.Equals after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }
    // Verify ValueType.GetHashCode after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32() { _ = ((ValueType)42).GetHashCode(); }
    // Verify ValueType.ToString after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ValueType_ToString_System_String() { _ = ((ValueType)42).ToString(); }}
