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

// Auto-generated benchmark skeletons for ReflectionActivationBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionActivationBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Activator.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type() { _ = Activator.CreateInstance(typeof(byte)); }
    // Purpose: Benchmark native-runtime performance of Activator.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Object() { _ = Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>()); }
    // Purpose: Benchmark native-runtime performance of Activator.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Activator_CreateInstance_T() { _ = Activator.CreateInstance(); }
    // Purpose: Benchmark native-runtime performance of Activator.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_R() { _ = Activator.CreateInstance(typeof(byte), null!, null!, Array.Empty<System.Object>(), null!); }
    // Purpose: Benchmark native-runtime performance of Array.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }
    // Purpose: Benchmark native-runtime performance of Array.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }
    // Purpose: Benchmark native-runtime performance of Array.CreateInstance with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()); }
    // Purpose: Benchmark native-runtime performance of ConstructorInfo.Invoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { _ = typeof(object).GetConstructor(System.Type.EmptyTypes)!.Invoke(null); }}
