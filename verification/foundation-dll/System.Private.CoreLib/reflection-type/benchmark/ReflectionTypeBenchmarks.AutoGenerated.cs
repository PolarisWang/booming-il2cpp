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

// Auto-generated benchmark skeletons for ReflectionTypeBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionTypeBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { _ = Type.GetType("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { _ = Type.GetType("hello", true); }
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { _ = Type.GetType("hello", true, true); }
    // Purpose: Benchmark native-runtime performance of Type.GetTypeFromHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Purpose: Benchmark native-runtime performance of Type.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }
    // Purpose: Benchmark native-runtime performance of Type.get_FullName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }
    // Purpose: Benchmark native-runtime performance of Type.get_Namespace with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Namespace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Namespace_System_String() { _ = typeof(byte).Namespace; }
    // Purpose: Benchmark native-runtime performance of Type.get_AssemblyQualifiedName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_AssemblyQualifiedName_System_String() { _ = typeof(byte).AssemblyQualifiedName; }
    // Purpose: Benchmark native-runtime performance of Type.get_TypeHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsValueType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsEnum with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsAbstract with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsAbstract_System_Boolean() { _ = typeof(byte).IsAbstract; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsSealed with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsSealed_System_Boolean() { _ = typeof(byte).IsSealed; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsInterface with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsInterface_System_Boolean() { _ = typeof(byte).IsInterface; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsByRef with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsByRef_System_Boolean() { _ = typeof(byte).IsByRef; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsPointer with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsPointer_System_Boolean() { _ = typeof(byte).IsPointer; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsGenericType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean() { _ = typeof(byte).IsGenericType; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsConstructedGenericType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean() { _ = typeof(byte).IsConstructedGenericType; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsNested with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNested_System_Boolean() { _ = typeof(byte).IsNested; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsPublic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsPublic_System_Boolean() { _ = typeof(byte).IsPublic; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsNotPublic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNotPublic_System_Boolean() { _ = typeof(byte).IsNotPublic; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsNestedPublic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNestedPublic_System_Boolean() { _ = typeof(byte).IsNestedPublic; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsNestedPrivate with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsNestedPrivate_System_Boolean() { _ = typeof(byte).IsNestedPrivate; }
    // Purpose: Benchmark native-runtime performance of Type.get_BaseType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }
    // Purpose: Benchmark native-runtime performance of Type.get_DeclaringType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_DeclaringType_System_Type() { _ = typeof(byte).DeclaringType; }
    // Purpose: Benchmark native-runtime performance of Type.get_ReflectedType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_ReflectedType_System_Type() { _ = typeof(byte).ReflectedType; }
    // Purpose: Benchmark native-runtime performance of Type.get_UnderlyingSystemType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_UnderlyingSystemType_System_Type() { _ = typeof(byte).UnderlyingSystemType; }
    // Purpose: Benchmark native-runtime performance of Type.GetMethods with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }
    // Purpose: Benchmark native-runtime performance of Type.GetMethods with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetMethods(null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetFields with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }
    // Purpose: Benchmark native-runtime performance of Type.GetFields with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetFields(null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetProperties with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo() { _ = typeof(byte).GetProperties(); }
    // Purpose: Benchmark native-runtime performance of Type.GetProperties with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetProperties(null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetEvents with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvents_System_Reflection_EventInfo() { _ = typeof(byte).GetEvents(); }
    // Purpose: Benchmark native-runtime performance of Type.GetConstructors with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo() { _ = typeof(byte).GetConstructors(); }
    // Purpose: Benchmark native-runtime performance of Type.GetConstructors with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo_System_Reflection_BindingFlags() { _ = typeof(byte).GetConstructors(null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetMembers with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMembers_System_Reflection_MemberInfo() { _ = typeof(byte).GetMembers(); }
    // Purpose: Benchmark native-runtime performance of Type.GetInterfaces with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetInterfaces_System_Type() { _ = typeof(byte).GetInterfaces(); }
    // Purpose: Benchmark native-runtime performance of Type.GetElementType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetElementType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetElementType_System_Type() { _ = typeof(byte).GetElementType(); }
    // Purpose: Benchmark native-runtime performance of Type.GetNestedTypes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetNestedTypes_System_Type() { _ = typeof(byte).GetNestedTypes(); }
    // Purpose: Benchmark native-runtime performance of Type.GetNestedType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetNestedType_System_Type_System_String() { _ = typeof(byte).GetNestedType("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.get_Assembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }}
