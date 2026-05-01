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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesPinvokeDllimportBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesPinvokeDllimportBenchmarks
{
    // Purpose: Benchmark native-runtime performance of DllImportAttribute.DllImportAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::DllImportAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_DllImportAttribute_Void_System_Object() { _ = DllImportAttribute.DllImportAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of DllImportAttribute.get_Value with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_get_Value_System_Void() { DllImportAttribute.Value; }
    // Purpose: Benchmark native-runtime performance of DllImportAttribute.set_Value with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_set_Value_System_Void_P0() { DllImportAttribute.Value = null!; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.LibraryImportAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::LibraryImportAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_LibraryImportAttribute_Void_System_() { _ = LibraryImportAttribute.LibraryImportAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.get_EntryPoint with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_EntryPoint_System_Void() { LibraryImportAttribute.EntryPoint; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.get_LibraryName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_LibraryName_System_Void() { LibraryImportAttribute.LibraryName; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.get_SetLastError with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_SetLastError_System_Void() { LibraryImportAttribute.SetLastError; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.get_StringMarshalling with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_StringMarshalling_System_Void() { LibraryImportAttribute.StringMarshalling; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.get_StringMarshallingCustomType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_StringMarshallingCustomType_Sys() { LibraryImportAttribute.StringMarshallingCustomType; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.set_EntryPoint with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_EntryPoint_System_Void_P0() { LibraryImportAttribute.EntryPoint = null!; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.set_SetLastError with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_SetLastError_System_Void_P0() { LibraryImportAttribute.SetLastError = null!; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.set_StringMarshalling with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_StringMarshalling_System_Void_P() { LibraryImportAttribute.StringMarshalling = null!; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.set_StringMarshallingCustomType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_StringMarshallingCustomType_Sys() { LibraryImportAttribute.StringMarshallingCustomType = null!; }
    // Purpose: Benchmark native-runtime performance of LibraryImportAttribute.set_LibraryName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_LibraryName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_LibraryName_System_Void_P0() { LibraryImportAttribute.LibraryName = null!; }}
