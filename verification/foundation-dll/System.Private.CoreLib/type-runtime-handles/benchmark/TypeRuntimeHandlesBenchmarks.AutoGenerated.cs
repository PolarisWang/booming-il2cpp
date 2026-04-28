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

// Auto-generated benchmark skeletons for TypeRuntimeHandlesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class TypeRuntimeHandlesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { }
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { }
    // Purpose: Benchmark native-runtime performance of Type.GetTypeFromHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }
    // Purpose: Benchmark native-runtime performance of Type.get_IsValueType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsEnum with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }
    // Purpose: Benchmark native-runtime performance of Type.get_IsArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }
    // Purpose: Benchmark native-runtime performance of Type.get_TypeHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }
    // Purpose: Benchmark native-runtime performance of Type.get_FullName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }
    // Purpose: Benchmark native-runtime performance of Type.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }
    // Purpose: Benchmark native-runtime performance of Type.get_BaseType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }
    // Purpose: Benchmark native-runtime performance of Type.GetMethods with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }
    // Purpose: Benchmark native-runtime performance of Type.GetFields with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }}
