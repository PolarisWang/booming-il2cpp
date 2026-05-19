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

// Benchmark stubs for family/System.Private.CoreLib/reflection/binding
public static class ReflectionBindingBenchmarks
{
    // Benchmark Type.GetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String() { _ = typeof(byte).GetMethod("hello"); }
    // Benchmark Type.GetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Type() { _ = typeof(byte).GetMethod("hello", Array.Empty<System.Type>()); }
    // Benchmark Type.GetField native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String() { _ = typeof(byte).GetField("hello"); }
    // Benchmark Type.GetField native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetProperty native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String() { _ = typeof(byte).GetProperty("hello"); }
    // Benchmark Type.GetProperty native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFla() { _ = typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default); }
    // Benchmark Type.GetProperty native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Type() { _ = typeof(byte).GetProperty("hello", Array.Empty<System.Type>()); }
    // Benchmark Type.GetEvent native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String() { _ = typeof(byte).GetEvent("hello"); }
    // Benchmark Type.GetEvent native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags() { _ = typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default); }
    // Benchmark MethodBase.Invoke native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object() { }
    // Benchmark MethodBase.Invoke native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionBinding)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Reflection_BindingFlags_Sy() { _ = default(MethodInfo)!.Invoke(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }}
