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

// Hot-update stubs for family/System.Private.CoreLib/boxing/unboxing-casts
public static class BoxingUnboxingCastsHotUpdate
{
    // Verify Convert.ChangeType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type() { _ = Convert.ChangeType(42, typeof(byte)); }
    // Verify Convert.ChangeType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider() { _ = Convert.ChangeType(42, typeof(byte), null); }
    // Verify Type.IsAssignableFrom after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableFrom(typeof(byte)); }
    // Verify Type.IsInstanceOfType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object() { _ = typeof(byte).IsInstanceOfType(42); }
    // Verify Type.IsSubclassOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type() { _ = typeof(byte).IsSubclassOf(typeof(byte)); }
    // Verify Type.IsAssignableTo after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableTo(typeof(byte)); }
    // Verify Object.MemberwiseClone after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }
    // Verify RuntimeHelpers.GetObjectValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }
    // Verify Array.CreateInstance after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }
    // Verify Array.CreateInstance after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }
    // Verify ValueType.Equals after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }}
