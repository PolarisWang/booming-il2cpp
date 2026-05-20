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

// Hot-update stubs for family/System.Private.CoreLib/reflection/member-complete
public static class ReflectionMemberCompleteHotUpdate
{
    // Verify MemberInfo.get_Name after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { _ = default(MemberInfo)!.Name; }
    // Verify MemberInfo.get_MemberType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { _ = typeof(byte).MemberType; }
    // Verify MemberInfo.get_DeclaringType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { _ = default(MemberInfo)!.DeclaringType; }
    // Verify MemberInfo.get_ReflectedType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type() { _ = default(MemberInfo)!.ReflectedType; }
    // Verify MemberInfo.get_MetadataToken after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32() { _ = default(MemberInfo)!.MetadataToken; }
    // Verify MethodInfo.get_ReturnType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { _ = typeof(byte).GetMethods()[0].ReturnType; }
    // Verify MethodInfo.GetBaseDefinition after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo() { _ = default(MethodInfo)!.GetBaseDefinition(); }
    // Verify MethodBase.get_MethodHandle after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle() { _ = default(MethodInfo)!.MethodHandle; }
    // Verify MethodBase.get_CallingConvention after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions() { _ = default(MethodInfo)!.CallingConvention; }
    // Verify MethodBase.get_IsPublic after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean() { _ = default(MethodInfo)!.IsPublic; }
    // Verify MethodBase.get_IsStatic after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean() { _ = default(MethodInfo)!.IsStatic; }
    // Verify MethodBase.get_IsVirtual after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean() { _ = default(MethodInfo)!.IsVirtual; }
    // Verify FieldInfo.get_FieldType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { _ = typeof(byte).GetFields()[0].FieldType; }
    // Verify FieldInfo.get_IsStatic after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean() { _ = default(FieldInfo)!.IsStatic; }
    // Verify FieldInfo.get_IsInitOnly after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean() { _ = default(FieldInfo)!.IsInitOnly; }
    // Verify FieldInfo.get_IsLiteral after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean() { _ = default(FieldInfo)!.IsLiteral; }
    // Verify PropertyInfo.get_PropertyType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { _ = typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].PropertyType; }
    // Verify PropertyInfo.get_CanRead after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean() { _ = default(PropertyInfo)!.CanRead; }
    // Verify PropertyInfo.get_CanWrite after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean() { _ = default(PropertyInfo)!.CanWrite; }
    // Verify PropertyInfo.GetIndexParameters after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo() { _ = default(PropertyInfo)!.GetIndexParameters(); }
    // Verify FieldInfo.get_FieldHandle after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle() { _ = default(FieldInfo)!.FieldHandle; }
    // Verify ConstructorInfo.get_CallingConvention after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions() { _ = default(ConstructorInfo)!.CallingConvention; }}
