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

// Hot-update stubs for family/System.Private.CoreLib/reflection/member-basics
public static class ReflectionMemberBasicsHotUpdate
{
    // Verify MemberInfo.get_Name after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { _ = default(MemberInfo)!.Name; }
    // Verify MemberInfo.get_MemberType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { _ = default(MemberInfo)!.MemberType; }
    // Verify MemberInfo.get_DeclaringType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { _ = default(MemberInfo)!.DeclaringType; }
    // Verify MethodBase.Invoke after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object() { }
    // Verify MethodInfo.GetParameters after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_GetParameters_System_Reflection_ParameterInfo() { }
    // Verify MethodInfo.get_ReturnType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { _ = default(MethodInfo)!.ReturnType; }
    // Verify ConstructorInfo.Invoke after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { _ = typeof(byte).GetConstructors()[0].Invoke(new object[0]); }
    // Verify FieldInfo.GetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_GetValue_System_Object_System_Object() { _ = typeof(byte).GetFields()[0].GetValue(null); }
    // Verify FieldInfo.SetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object() { typeof(byte).GetFields()[0].SetValue(null, (byte)42); }
    // Verify FieldInfo.get_FieldType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { _ = 0; }
    // Verify PropertyInfo.GetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object() { _ = typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null); }
    // Verify PropertyInfo.GetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Object() { _ = typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null); }
    // Verify PropertyInfo.SetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object() { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].SetValue(null, (byte)42); }
    // Verify PropertyInfo.get_PropertyType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { _ = 0; }}
