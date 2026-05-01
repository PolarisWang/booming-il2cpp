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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.zipfileZipFileExtensionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.zipfileZipFileExtensionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.ZipFileExtensions: with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ZipFileExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_ZipFileExtensions_Void_System_Object() { _ = ZipFileExtensions.ZipFileExtensions:(42); }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Property_System_Void() { ZipFileExtensions.Property; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method1_System_Void_P0() { ZipFileExtensions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method2_System_Void_P0_P1() { ZipFileExtensions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Validate_System_Void_P0() { ZipFileExtensions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Value_System_Void() { ZipFileExtensions.Value; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Value_System_Void_P0() { ZipFileExtensions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Count_System_Void() { ZipFileExtensions.Count; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Count_System_Void_P0() { ZipFileExtensions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_IsEmpty_System_Void() { ZipFileExtensions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_IsEmpty_System_Void_P0() { ZipFileExtensions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Key_System_Void() { ZipFileExtensions.Key; }
    // Purpose: Benchmark native-runtime performance of ZipFileExtensions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Key_System_Void_P0() { ZipFileExtensions.Key = null!; }}
