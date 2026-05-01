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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.brotliDecoderBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.brotliDecoderBenchmarks
{
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.BrotliDecoder: with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::BrotliDecoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_BrotliDecoder_Void_System_Object() { _ = BrotliDecoder.BrotliDecoder:(42); }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Property_System_Void() { BrotliDecoder.Property; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Method1_System_Void_P0() { BrotliDecoder.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Method2_System_Void_P0_P1() { BrotliDecoder.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_Validate_System_Void_P0() { BrotliDecoder.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Value_System_Void() { BrotliDecoder.Value; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Value_System_Void_P0() { BrotliDecoder.Value = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Count_System_Void() { BrotliDecoder.Count; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Count_System_Void_P0() { BrotliDecoder.Count = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_IsEmpty_System_Void() { BrotliDecoder.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_IsEmpty_System_Void_P0() { BrotliDecoder.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_get_Key_System_Void() { BrotliDecoder.Key; }
    // Purpose: Benchmark native-runtime performance of BrotliDecoder.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoder_set_Key_System_Void_P0() { BrotliDecoder.Key = null!; }}
