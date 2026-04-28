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

// Auto-generated patch-side skeletons for ReflectionMemberBasicsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ReflectionMemberBasicsPatch
{
    // Purpose: Verify MemberInfo.get_Name executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { }
    // Purpose: Verify MemberInfo.get_MemberType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { }
    // Purpose: Verify MemberInfo.get_DeclaringType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { }
    // Purpose: Verify MethodBase.Invoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object() { }
    // Purpose: Verify MethodInfo.GetParameters executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MethodInfo_GetParameters_System_Reflection_ParameterInfo() { }
    // Purpose: Verify MethodInfo.get_ReturnType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { }
    // Purpose: Verify ConstructorInfo.Invoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { }
    // Purpose: Verify FieldInfo.GetValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_FieldInfo_GetValue_System_Object_System_Object() { }
    // Purpose: Verify FieldInfo.SetValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object() { }
    // Purpose: Verify FieldInfo.get_FieldType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { }
    // Purpose: Verify PropertyInfo.GetValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object() { }
    // Purpose: Verify PropertyInfo.GetValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Object() { }
    // Purpose: Verify PropertyInfo.SetValue executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object() { }
    // Purpose: Verify PropertyInfo.get_PropertyType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { }}
