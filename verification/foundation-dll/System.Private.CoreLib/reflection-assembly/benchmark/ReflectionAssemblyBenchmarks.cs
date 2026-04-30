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

// Auto-generated benchmark skeletons for ReflectionAssemblyBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionAssemblyBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Assembly.GetExecutingAssembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetExecutingAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetExecutingAssembly(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetCallingAssembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetCallingAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetCallingAssembly(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetEntryAssembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetEntryAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetEntryAssembly(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetName_System_Reflection_AssemblyName() { _ = typeof(byte).Assembly.GetName(); }
    // Purpose: Benchmark native-runtime performance of Assembly.get_FullName with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_FullName_System_String() { _ = typeof(byte).Assembly.FullName; }
    // Purpose: Benchmark native-runtime performance of Assembly.get_Location with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_Location_System_String() { _ = typeof(byte).Assembly.Location; }
    // Purpose: Benchmark native-runtime performance of Assembly.get_ImageRuntimeVersion with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_ImageRuntimeVersion_System_String() { _ = typeof(byte).Assembly.ImageRuntimeVersion; }
    // Purpose: Benchmark native-runtime performance of Assembly.GetTypes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type() { _ = typeof(byte).Assembly.GetTypes(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetExportedTypes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetExportedTypes_System_Type() { _ = typeof(byte).Assembly.GetExportedTypes(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetForwardedTypes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetForwardedTypes_System_Type() { _ = typeof(byte).Assembly.GetForwardedTypes(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String() { _ = typeof(byte).Assembly.GetType("hello"); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String_System_Boolean() { _ = typeof(byte).Assembly.GetType("hello", true); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetModule with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetModule_System_Reflection_Module_System_String() { _ = typeof(byte).Assembly.GetModule("hello"); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetModules with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module() { _ = typeof(byte).Assembly.GetModules(); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetManifestResourceStream with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceStream_System_IO_Stream_System_String() { _ = typeof(byte).Assembly.GetManifestResourceStream("hello"); }
    // Purpose: Benchmark native-runtime performance of Assembly.GetManifestResourceNames with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceNames_System_String() { _ = typeof(byte).Assembly.GetManifestResourceNames(); }
    // Purpose: Benchmark native-runtime performance of Assembly.get_EntryPoint with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_Assembly_get_EntryPoint_System_Reflection_MethodInfo() { _ = typeof(byte).Assembly.EntryPoint; }
    // Purpose: Benchmark native-runtime performance of AssemblyName.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_Name_System_String() { _ = AssemblyName.Name; }
    // Purpose: Benchmark native-runtime performance of AssemblyName.get_Version with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_Version_System_Version() { _ = AssemblyName.Version; }
    // Purpose: Benchmark native-runtime performance of AssemblyName.get_CultureInfo with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_AssemblyName_get_CultureInfo_System_Globalization_CultureInfo() { _ = AssemblyName.CultureInfo; }
    // Purpose: Benchmark native-runtime performance of Type.get_Assembly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }
    // Purpose: Benchmark native-runtime performance of Type.GetType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { _ = Type.GetType("hello"); }}
