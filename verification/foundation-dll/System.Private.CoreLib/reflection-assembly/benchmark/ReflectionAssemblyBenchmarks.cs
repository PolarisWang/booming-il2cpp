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

// Benchmark stubs for family/System.Private.CoreLib/reflection/assembly
public static class ReflectionAssemblyBenchmarks
{
    // Benchmark Assembly.GetExecutingAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetExecutingAssembly_System_Reflection_Assembly() { _ = Assembly.GetExecutingAssembly(); }
    // Benchmark Assembly.GetCallingAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetCallingAssembly_System_Reflection_Assembly() { _ = Assembly.GetCallingAssembly(); }
    // Benchmark Assembly.GetEntryAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetEntryAssembly_System_Reflection_Assembly() { _ = Assembly.GetEntryAssembly(); }
    // Benchmark Assembly.GetName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetName_System_Reflection_AssemblyName() { _ = typeof(byte).Assembly.GetName(); }
    // Benchmark Assembly.get_FullName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_FullName_System_String() { _ = typeof(byte).Assembly.FullName; }
    // Benchmark Assembly.get_Location native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_Location_System_String() { _ = typeof(byte).Assembly.Location; }
    // Benchmark Assembly.get_ImageRuntimeVersion native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_ImageRuntimeVersion_System_String() { _ = typeof(byte).Assembly.ImageRuntimeVersion; }
    // Benchmark Assembly.GetTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type() { _ = typeof(byte).Assembly.GetTypes(); }
    // Benchmark Assembly.GetExportedTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetExportedTypes_System_Type() { _ = typeof(byte).Assembly.GetExportedTypes(); }
    // Benchmark Assembly.GetForwardedTypes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetForwardedTypes_System_Type() { _ = typeof(byte).Assembly.GetForwardedTypes(); }
    // Benchmark Assembly.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String() { _ = typeof(byte).Assembly.GetType("hello"); }
    // Benchmark Assembly.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String_System_Boolean() { _ = typeof(byte).Assembly.GetType("hello", true); }
    // Benchmark Assembly.GetModule native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetModule_System_Reflection_Module_System_String() { _ = typeof(byte).Assembly.GetModule("hello"); }
    // Benchmark Assembly.GetModules native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module() { _ = typeof(byte).Assembly.GetModules(); }
    // Benchmark Assembly.GetManifestResourceStream native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceStream_System_IO_Stream_System_String() { _ = typeof(byte).Assembly.GetManifestResourceStream("hello"); }
    // Benchmark Assembly.GetManifestResourceNames native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceNames_System_String() { _ = typeof(byte).Assembly.GetManifestResourceNames(); }
    // Benchmark Assembly.get_EntryPoint native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_EntryPoint_System_Reflection_MethodInfo() { _ = typeof(byte).Assembly.EntryPoint; }
    // Benchmark AssemblyName.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_Name_System_String() { _ = default(AssemblyName)!.Name; }
    // Benchmark AssemblyName.get_Version native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_Version_System_Version() { _ = default(AssemblyName)!.Version; }
    // Benchmark AssemblyName.get_CultureInfo native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_CultureInfo_System_Globalization_CultureInfo() { _ = default(AssemblyName)!.CultureInfo; }
    // Benchmark Type.get_Assembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }
    // Benchmark Type.GetType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { _ = Type.GetType("System.Int32"); }}
