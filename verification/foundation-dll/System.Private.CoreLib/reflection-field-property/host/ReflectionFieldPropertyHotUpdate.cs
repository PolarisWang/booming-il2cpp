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

// Hot-update stubs for family/System.Private.CoreLib/reflection/field-property
public static class ReflectionFieldPropertyHotUpdate
{
    // Verify FieldInfo.get_MemberType after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_MemberType_System_Reflection_MemberTypes() { _ = default(FieldInfo)!.MemberType; }
    // Verify FieldInfo.get_Attributes after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_Attributes:System.Reflection.FieldAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_Attributes_System_Reflection_FieldAttributes() { _ = default(FieldInfo)!.Attributes; }
    // Verify FieldInfo.get_IsNotSerialized after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsNotSerialized:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsNotSerialized_System_Boolean() { _ = default(FieldInfo)!.IsNotSerialized; }
    // Verify FieldInfo.get_IsPinvokeImpl after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPinvokeImpl:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPinvokeImpl_System_Boolean() { _ = default(FieldInfo)!.IsPinvokeImpl; }
    // Verify FieldInfo.get_IsSpecialName after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSpecialName:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSpecialName_System_Boolean() { _ = default(FieldInfo)!.IsSpecialName; }
    // Verify FieldInfo.get_IsAssembly after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsAssembly_System_Boolean() { _ = default(FieldInfo)!.IsAssembly; }
    // Verify FieldInfo.get_IsFamily after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamily:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamily_System_Boolean() { _ = default(FieldInfo)!.IsFamily; }
    // Verify FieldInfo.get_IsFamilyAndAssembly after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamilyAndAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamilyAndAssembly_System_Boolean() { _ = default(FieldInfo)!.IsFamilyAndAssembly; }
    // Verify FieldInfo.get_IsFamilyOrAssembly after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamilyOrAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamilyOrAssembly_System_Boolean() { _ = default(FieldInfo)!.IsFamilyOrAssembly; }
    // Verify FieldInfo.get_IsPrivate after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPrivate_System_Boolean() { _ = default(FieldInfo)!.IsPrivate; }
    // Verify FieldInfo.get_IsPublic after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPublic_System_Boolean() { _ = default(FieldInfo)!.IsPublic; }
    // Verify FieldInfo.get_IsSecurityCritical after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecurityCritical:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecurityCritical_System_Boolean() { _ = default(FieldInfo)!.IsSecurityCritical; }
    // Verify FieldInfo.get_IsSecuritySafeCritical after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecuritySafeCritical:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecuritySafeCritical_System_Boolean() { _ = default(FieldInfo)!.IsSecuritySafeCritical; }
    // Verify FieldInfo.get_IsSecurityTransparent after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecurityTransparent:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecurityTransparent_System_Boolean() { _ = default(FieldInfo)!.IsSecurityTransparent; }
    // Verify FieldInfo.SetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object_System_Reflection_Bi() { default(FieldInfo)!.SetValue(42, 42, System.Reflection.BindingFlags.Default, null!, null!); }
    // Verify FieldInfo.GetRawConstantValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetRawConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_GetRawConstantValue_System_Object() { _ = default(FieldInfo)!.GetRawConstantValue(); }
    // Verify FieldInfo.GetOptionalCustomModifiers after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_GetOptionalCustomModifiers_System_Type() { _ = default(FieldInfo)!.GetOptionalCustomModifiers(); }
    // Verify FieldInfo.GetRequiredCustomModifiers after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_FieldInfo_GetRequiredCustomModifiers_System_Type() { _ = default(FieldInfo)!.GetRequiredCustomModifiers(); }
    // Verify PropertyInfo.get_Attributes after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_Attributes:System.Reflection.PropertyAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_Attributes_System_Reflection_PropertyAttributes() { _ = default(PropertyInfo)!.Attributes; }
    // Verify PropertyInfo.get_IsSpecialName after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_IsSpecialName:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_IsSpecialName_System_Boolean() { _ = default(PropertyInfo)!.IsSpecialName; }
    // Verify PropertyInfo.GetAccessors after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetAccessors:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetAccessors_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetAccessors(); }
    // Verify PropertyInfo.GetAccessors after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetAccessors:System.Reflection.MethodInfo[](System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetAccessors_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetAccessors(true); }
    // Verify PropertyInfo.get_GetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_GetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_GetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetMethod; }
    // Verify PropertyInfo.GetGetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetGetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetGetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetGetMethod(); }
    // Verify PropertyInfo.GetGetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetGetMethod:System.Reflection.MethodInfo(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetGetMethod_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetGetMethod(true); }
    // Verify PropertyInfo.get_SetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_SetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_get_SetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.SetMethod; }
    // Verify PropertyInfo.GetSetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetSetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetSetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetSetMethod(); }
    // Verify PropertyInfo.GetSetMethod after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetSetMethod:System.Reflection.MethodInfo(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetSetMethod_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetSetMethod(true); }
    // Verify PropertyInfo.GetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Reflection_BindingFlag() { _ = default(PropertyInfo)!.GetValue(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }
    // Verify PropertyInfo.GetConstantValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetConstantValue_System_Object() { _ = default(PropertyInfo)!.GetConstantValue(); }
    // Verify PropertyInfo.GetRawConstantValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetRawConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetRawConstantValue_System_Object() { _ = default(PropertyInfo)!.GetRawConstantValue(); }
    // Verify PropertyInfo.SetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object_System_Reflection() { default(PropertyInfo)!.SetValue(42, 42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }
    // Verify PropertyInfo.GetOptionalCustomModifiers after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetOptionalCustomModifiers_System_Type() { _ = default(PropertyInfo)!.GetOptionalCustomModifiers(); }
    // Verify PropertyInfo.GetRequiredCustomModifiers after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_PropertyInfo_GetRequiredCustomModifiers_System_Type() { _ = default(PropertyInfo)!.GetRequiredCustomModifiers(); }}
