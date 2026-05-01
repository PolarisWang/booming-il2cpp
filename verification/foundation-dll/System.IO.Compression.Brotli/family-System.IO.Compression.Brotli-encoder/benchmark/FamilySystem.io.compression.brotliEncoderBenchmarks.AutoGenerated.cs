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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.brotliEncoderBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.brotliEncoderBenchmarks
{
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.BrotliEncoder: with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::BrotliEncoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_BrotliEncoder_Void_System_Object() { _ = BrotliEncoder.BrotliEncoder:(42); }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Property_System_Void() { BrotliEncoder.Property; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method1_System_Void_P0() { BrotliEncoder.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method2_System_Void_P0_P1() { BrotliEncoder.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Validate_System_Void_P0() { BrotliEncoder.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Value_System_Void() { BrotliEncoder.Value; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Value_System_Void_P0() { BrotliEncoder.Value = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Count_System_Void() { BrotliEncoder.Count; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Count_System_Void_P0() { BrotliEncoder.Count = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_IsEmpty_System_Void() { BrotliEncoder.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_IsEmpty_System_Void_P0() { BrotliEncoder.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Key_System_Void() { BrotliEncoder.Key; }
    // Purpose: Benchmark native-runtime performance of BrotliEncoder.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Key_System_Void_P0() { BrotliEncoder.Key = null!; }}
