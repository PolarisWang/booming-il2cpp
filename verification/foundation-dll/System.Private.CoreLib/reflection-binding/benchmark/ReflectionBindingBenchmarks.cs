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

// Auto-generated benchmark skeletons for ReflectionBindingBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionBindingBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Type.GetMethod with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String() { _ = typeof(byte).GetMethod("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.GetMethod with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetMethod("hello", null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetMethod with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Type() { _ = typeof(byte).GetMethod("hello", Array.Empty<System.Type>()); }
    // Purpose: Benchmark native-runtime performance of Type.GetField with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String() { _ = typeof(byte).GetField("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.GetField with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetField("hello", null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetProperty with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String() { _ = typeof(byte).GetProperty("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.GetProperty with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFla() { _ = typeof(byte).GetProperty("hello", null!); }
    // Purpose: Benchmark native-runtime performance of Type.GetProperty with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Type() { _ = typeof(byte).GetProperty("hello", Array.Empty<System.Type>()); }
    // Purpose: Benchmark native-runtime performance of Type.GetEvent with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String() { _ = typeof(byte).GetEvent("hello"); }
    // Purpose: Benchmark native-runtime performance of Type.GetEvent with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetEvent("hello", null!); }
    // Purpose: Benchmark native-runtime performance of MethodBase.Invoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.Invoke("hello", null); }
    // Purpose: Benchmark native-runtime performance of MethodBase.Invoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Reflection_BindingFlags_Sy() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.Invoke(42, null!, null!, Array.Empty<System.Object>(), null!); }}
