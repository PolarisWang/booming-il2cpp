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

// Auto-generated patch-side skeletons for TypeRuntimeHandlesPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class TypeRuntimeHandlesPatch
{
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { }
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { }
    // Purpose: Verify Type.GetTypeFromHandle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Purpose: Verify Type.get_IsValueType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }
    // Purpose: Verify Type.get_IsEnum executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }
    // Purpose: Verify Type.get_IsArray executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }
    // Purpose: Verify Type.get_TypeHandle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }
    // Purpose: Verify Type.get_FullName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }
    // Purpose: Verify Type.get_Name executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }
    // Purpose: Verify Type.get_BaseType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }
    // Purpose: Verify Type.GetMethods executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }
    // Purpose: Verify Type.GetFields executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }}
