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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.brotliOptionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.brotliOptionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.BrotliCompressionOptions: with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::BrotliCompressionOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_BrotliCompressionOptions_Void_System_Object() { _ = BrotliCompressionOptions.BrotliCompressionOptions:(42); }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_get_Property_System_Void() { BrotliCompressionOptions.Property; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_Method1_System_Void_P0() { BrotliCompressionOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_Method2_System_Void_P0_P1() { BrotliCompressionOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_Validate_System_Void_P0() { BrotliCompressionOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_get_Value_System_Void() { BrotliCompressionOptions.Value; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_set_Value_System_Void_P0() { BrotliCompressionOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_get_Count_System_Void() { BrotliCompressionOptions.Count; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_set_Count_System_Void_P0() { BrotliCompressionOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_get_IsEmpty_System_Void() { BrotliCompressionOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_set_IsEmpty_System_Void_P0() { BrotliCompressionOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_get_Key_System_Void() { BrotliCompressionOptions.Key; }
    // Purpose: Benchmark native-runtime performance of BrotliCompressionOptions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliCompressionOptions_set_Key_System_Void_P0() { BrotliCompressionOptions.Key = null!; }}
