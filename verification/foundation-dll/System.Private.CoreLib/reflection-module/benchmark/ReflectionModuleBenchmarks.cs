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

// Benchmark stubs for family/System.Private.CoreLib/reflection/module
public static class ReflectionModuleBenchmarks
{
    // Benchmark Module.get_FullyQualifiedName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_FullyQualifiedName_System_String() { _ = default(Module)!.FullyQualifiedName; }
    // Benchmark Module.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_Name_System_String() { _ = default(Module)!.Name; }
    // Benchmark Module.get_Assembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_get_Assembly_System_Reflection_Assembly() { _ = default(Module)!.Assembly; }
    // Benchmark Module.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetType_System_Type_System_String() { _ = default(Module)!.GetType("hello"); }
    // Benchmark Module.GetTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetTypes_System_Type() { _ = default(Module)!.GetTypes(); }
    // Benchmark Module.GetCustomAttributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Module_GetCustomAttributes_System_Object_System_Type() { _ = new List<Attribute>(typeof(byte).Module.GetCustomAttributes(false)).Count; }}
