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

// Benchmark stubs for family/System.Private.CoreLib/reflection/type
public static class ReflectionTypeBenchmarks
{
    // Benchmark Type.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { _ = Type.GetType("System.Int32"); }
    // Benchmark Type.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { _ = Type.GetType("System.Int32", false); }
    // Benchmark Type.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { _ = Type.GetType("System.Int32", false, false); }
    // Benchmark Type.GetTypeFromHandle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Benchmark Type.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }
    // Benchmark Type.get_FullName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }
    // Benchmark Type.get_Namespace native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Namespace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Namespace_System_String() { _ = typeof(byte).Namespace; }
    // Benchmark Type.get_AssemblyQualifiedName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_AssemblyQualifiedName_System_String() { _ = typeof(byte).AssemblyQualifiedName; }
    // Benchmark Type.get_TypeHandle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }
    // Benchmark Type.get_IsValueType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }
    // Benchmark Type.get_IsEnum native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }
    // Benchmark Type.get_IsAbstract native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsAbstract_System_Boolean() { _ = typeof(byte).IsAbstract; }
    // Benchmark Type.get_IsSealed native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsSealed_System_Boolean() { _ = typeof(byte).IsSealed; }
    // Benchmark Type.get_IsInterface native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsInterface_System_Boolean() { _ = typeof(byte).IsInterface; }
    // Benchmark Type.get_IsArray native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }
    // Benchmark Type.get_IsByRef native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsByRef_System_Boolean() { _ = typeof(byte).IsByRef; }
    // Benchmark Type.get_IsPointer native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsPointer_System_Boolean() { _ = typeof(byte).IsPointer; }
    // Benchmark Type.get_IsGenericType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean() { _ = typeof(byte).IsGenericType; }
    // Benchmark Type.get_IsConstructedGenericType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean() { _ = typeof(byte).IsConstructedGenericType; }
    // Benchmark Type.get_IsNested native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNested_System_Boolean() { _ = typeof(byte).IsNested; }
    // Benchmark Type.get_IsPublic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsPublic_System_Boolean() { _ = typeof(byte).IsPublic; }
    // Benchmark Type.get_IsNotPublic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNotPublic_System_Boolean() { _ = typeof(byte).IsNotPublic; }
    // Benchmark Type.get_IsNestedPublic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNestedPublic_System_Boolean() { _ = typeof(byte).IsNestedPublic; }
    // Benchmark Type.get_IsNestedPrivate native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNestedPrivate_System_Boolean() { _ = typeof(byte).IsNestedPrivate; }
    // Benchmark Type.get_BaseType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }
    // Benchmark Type.get_DeclaringType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_DeclaringType_System_Type() { _ = typeof(byte).DeclaringType; }
    // Benchmark Type.get_ReflectedType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_ReflectedType_System_Type() { _ = typeof(byte).ReflectedType; }
    // Benchmark Type.get_UnderlyingSystemType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_UnderlyingSystemType_System_Type() { _ = typeof(byte).UnderlyingSystemType; }
    // Benchmark Type.GetMethods native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }
    // Benchmark Type.GetMethods native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetMethods(System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetFields native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }
    // Benchmark Type.GetFields native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetFields(System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetProperties native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo() { _ = typeof(byte).GetProperties(); }
    // Benchmark Type.GetProperties native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetProperties(System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetEvents native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvents_System_Reflection_EventInfo() { _ = typeof(byte).GetEvents(); }
    // Benchmark Type.GetConstructors native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo() { _ = typeof(byte).GetConstructors(); }
    // Benchmark Type.GetConstructors native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetConstructors(System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetMembers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMembers_System_Reflection_MemberInfo() { _ = typeof(byte).GetMembers(); }
    // Benchmark Type.GetInterfaces native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetInterfaces_System_Type() { _ = typeof(byte).GetInterfaces(); }
    // Benchmark Type.GetElementType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetElementType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetElementType_System_Type() { _ = typeof(byte).GetElementType(); }
    // Benchmark Type.GetNestedTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetNestedTypes_System_Type() { _ = typeof(byte).GetNestedTypes(); }
    // Benchmark Type.GetNestedType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetNestedType_System_Type_System_String() { _ = typeof(byte).GetNestedType("hello"); }
    // Benchmark Type.get_Assembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }}
