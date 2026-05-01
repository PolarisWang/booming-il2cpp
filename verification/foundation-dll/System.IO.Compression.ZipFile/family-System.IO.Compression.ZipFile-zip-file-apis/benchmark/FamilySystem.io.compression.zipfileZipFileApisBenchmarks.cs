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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.zipfileZipFileApisBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.zipfileZipFileApisBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ZipFile.ZipFile: with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::ZipFile::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_ZipFile_Void_System_Object() { _ = ZipFile.ZipFile:(42); }
    // Purpose: Benchmark native-runtime performance of ZipFile.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Property_System_Void() { ZipFile.Property; }
    // Purpose: Benchmark native-runtime performance of ZipFile.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Method1_System_Void_P0() { ZipFile.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ZipFile.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Method2_System_Void_P0_P1() { ZipFile.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ZipFile.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_Validate_System_Void_P0() { ZipFile.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ZipFile.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Value_System_Void() { ZipFile.Value; }
    // Purpose: Benchmark native-runtime performance of ZipFile.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Value_System_Void_P0() { ZipFile.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFile.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Count_System_Void() { ZipFile.Count; }
    // Purpose: Benchmark native-runtime performance of ZipFile.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Count_System_Void_P0() { ZipFile.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFile.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_IsEmpty_System_Void() { ZipFile.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ZipFile.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_IsEmpty_System_Void_P0() { ZipFile.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ZipFile.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_get_Key_System_Void() { ZipFile.Key; }
    // Purpose: Benchmark native-runtime performance of ZipFile.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.ZipFile/System.IO.Compression.ZipFile::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_ZipFile_System_IO_Compression_ZipFile_set_Key_System_Void_P0() { ZipFile.Key = null!; }}
