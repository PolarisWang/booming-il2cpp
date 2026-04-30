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

// Auto-generated benchmark skeletons for ReflectionModuleBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionModuleBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Module.get_FullyQualifiedName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_FullyQualifiedName_System_String() { _ = Module.FullyQualifiedName; }
    // Purpose: Benchmark native-runtime performance of Module.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_Name_System_String() { _ = Module.Name; }
    // Purpose: Benchmark native-runtime performance of Module.get_Assembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_Assembly_System_Reflection_Assembly() { _ = Module.Assembly; }
    // Purpose: Benchmark native-runtime performance of Module.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetType_System_Type_System_String() { _ = Module.GetType("hello"); }
    // Purpose: Benchmark native-runtime performance of Module.GetTypes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetTypes_System_Type() { _ = Module.GetTypes(); }
    // Purpose: Benchmark native-runtime performance of Module.GetCustomAttributes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetCustomAttributes_System_Object_System_Type() { _ = Module.GetCustomAttributes(typeof(byte)); }}
