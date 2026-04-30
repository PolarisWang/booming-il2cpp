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

// Auto-generated hot-update skeletons for ReflectionMemberCompleteHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class ReflectionMemberCompleteHotUpdate
{
    // Purpose: Verify MemberInfo.get_Name executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { _ = typeof(string).GetMembers()[0].Name; }
    // Purpose: Verify MemberInfo.get_MemberType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { _ = typeof(string).GetMembers()[0].MemberType; }
    // Purpose: Verify MemberInfo.get_DeclaringType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { _ = typeof(string).GetMembers()[0].DeclaringType; }
    // Purpose: Verify MemberInfo.get_ReflectedType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type() { _ = typeof(string).GetMembers()[0].ReflectedType; }
    // Purpose: Verify MemberInfo.get_MetadataToken executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32() { _ = typeof(string).GetMembers()[0].MetadataToken; }
    // Purpose: Verify MethodInfo.get_ReturnType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.ReturnType; }
    // Purpose: Verify MethodInfo.GetBaseDefinition executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetBaseDefinition(); }
    // Purpose: Verify MethodBase.get_MethodHandle executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.MethodHandle; }
    // Purpose: Verify MethodBase.get_CallingConvention executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.CallingConvention; }
    // Purpose: Verify MethodBase.get_IsPublic executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsPublic; }
    // Purpose: Verify MethodBase.get_IsStatic executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsStatic; }
    // Purpose: Verify MethodBase.get_IsVirtual executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsVirtual; }
    // Purpose: Verify FieldInfo.get_FieldType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { _ = typeof(string).GetField("Empty")!.FieldType; }
    // Purpose: Verify FieldInfo.get_IsStatic executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsStatic; }
    // Purpose: Verify FieldInfo.get_IsInitOnly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsInitOnly; }
    // Purpose: Verify FieldInfo.get_IsLiteral executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsLiteral; }
    // Purpose: Verify PropertyInfo.get_PropertyType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { _ = typeof(string).GetProperty("Length")!.PropertyType; }
    // Purpose: Verify PropertyInfo.get_CanRead executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean() { _ = typeof(string).GetProperty("Length")!.CanRead; }
    // Purpose: Verify PropertyInfo.get_CanWrite executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean() { _ = typeof(string).GetProperty("Length")!.CanWrite; }
    // Purpose: Verify PropertyInfo.GetIndexParameters executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo() { _ = typeof(string).GetProperty("Length")!.GetIndexParameters(); }
    // Purpose: Verify FieldInfo.get_FieldHandle executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle() { _ = typeof(string).GetField("Empty")!.FieldHandle; }
    // Purpose: Verify ConstructorInfo.get_CallingConvention executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions() { _ = typeof(object).GetConstructor(System.Type.EmptyTypes)!.CallingConvention; }}
