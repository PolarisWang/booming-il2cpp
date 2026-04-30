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

// Auto-generated patch-side skeletons for ReflectionTypePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ReflectionTypePatch
{
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { }
    // Purpose: Verify Type.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { }
    // Purpose: Verify Type.GetTypeFromHandle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Purpose: Verify Type.get_Name executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }
    // Purpose: Verify Type.get_FullName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }
    // Purpose: Verify Type.get_Namespace executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Namespace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_Namespace_System_String() { _ = typeof(byte).Namespace; }
    // Purpose: Verify Type.get_AssemblyQualifiedName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_AssemblyQualifiedName_System_String() { _ = typeof(byte).AssemblyQualifiedName; }
    // Purpose: Verify Type.get_TypeHandle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }
    // Purpose: Verify Type.get_IsValueType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }
    // Purpose: Verify Type.get_IsEnum executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }
    // Purpose: Verify Type.get_IsAbstract executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsAbstract_System_Boolean() { _ = typeof(byte).IsAbstract; }
    // Purpose: Verify Type.get_IsSealed executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsSealed_System_Boolean() { _ = typeof(byte).IsSealed; }
    // Purpose: Verify Type.get_IsInterface executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsInterface_System_Boolean() { _ = typeof(byte).IsInterface; }
    // Purpose: Verify Type.get_IsArray executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }
    // Purpose: Verify Type.get_IsByRef executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsByRef_System_Boolean() { _ = typeof(byte).IsByRef; }
    // Purpose: Verify Type.get_IsPointer executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsPointer_System_Boolean() { _ = typeof(byte).IsPointer; }
    // Purpose: Verify Type.get_IsGenericType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean() { _ = typeof(byte).IsGenericType; }
    // Purpose: Verify Type.get_IsConstructedGenericType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean() { _ = typeof(byte).IsConstructedGenericType; }
    // Purpose: Verify Type.get_IsNested executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsNested_System_Boolean() { _ = typeof(byte).IsNested; }
    // Purpose: Verify Type.get_IsPublic executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsPublic_System_Boolean() { _ = typeof(byte).IsPublic; }
    // Purpose: Verify Type.get_IsNotPublic executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsNotPublic_System_Boolean() { _ = typeof(byte).IsNotPublic; }
    // Purpose: Verify Type.get_IsNestedPublic executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsNestedPublic_System_Boolean() { _ = typeof(byte).IsNestedPublic; }
    // Purpose: Verify Type.get_IsNestedPrivate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_IsNestedPrivate_System_Boolean() { _ = typeof(byte).IsNestedPrivate; }
    // Purpose: Verify Type.get_BaseType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }
    // Purpose: Verify Type.get_DeclaringType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_DeclaringType_System_Type() { _ = typeof(byte).DeclaringType; }
    // Purpose: Verify Type.get_ReflectedType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_ReflectedType_System_Type() { _ = typeof(byte).ReflectedType; }
    // Purpose: Verify Type.get_UnderlyingSystemType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_UnderlyingSystemType_System_Type() { _ = typeof(byte).UnderlyingSystemType; }
    // Purpose: Verify Type.GetMethods executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }
    // Purpose: Verify Type.GetMethods executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetMethods(null!); }
    // Purpose: Verify Type.GetFields executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }
    // Purpose: Verify Type.GetFields executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetFields(null!); }
    // Purpose: Verify Type.GetProperties executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo() { _ = typeof(byte).GetProperties(); }
    // Purpose: Verify Type.GetProperties executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetProperties(null!); }
    // Purpose: Verify Type.GetEvents executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetEvents_System_Reflection_EventInfo() { _ = typeof(byte).GetEvents(); }
    // Purpose: Verify Type.GetConstructors executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo() { _ = typeof(byte).GetConstructors(); }
    // Purpose: Verify Type.GetConstructors executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetConstructors(null!); }
    // Purpose: Verify Type.GetMembers executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetMembers_System_Reflection_MemberInfo() { _ = typeof(byte).GetMembers(); }
    // Purpose: Verify Type.GetInterfaces executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetInterfaces_System_Type() { _ = typeof(byte).GetInterfaces(); }
    // Purpose: Verify Type.GetElementType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetElementType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetElementType_System_Type() { _ = typeof(byte).GetElementType(); }
    // Purpose: Verify Type.GetNestedTypes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetNestedTypes_System_Type() { _ = typeof(byte).GetNestedTypes(); }
    // Purpose: Verify Type.GetNestedType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetNestedType_System_Type_System_String() { _ = typeof(byte).GetNestedType("hello"); }
    // Purpose: Verify Type.get_Assembly executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }}
