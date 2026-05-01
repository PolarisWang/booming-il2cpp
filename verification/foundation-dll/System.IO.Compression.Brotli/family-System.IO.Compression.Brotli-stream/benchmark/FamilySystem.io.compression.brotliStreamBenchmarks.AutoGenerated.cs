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

// Auto-generated benchmark skeletons for FamilySystem.io.compression.brotliStreamBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.compression.brotliStreamBenchmarks
{
    // Purpose: Benchmark native-runtime performance of BrotliStream.BrotliStream: with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BrotliStream::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_BrotliStream_Void_System_Object() { _ = BrotliStream.BrotliStream:(42); }
    // Purpose: Benchmark native-runtime performance of BrotliStream.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Property_System_Void() { BrotliStream.Property; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Method1_System_Void_P0() { BrotliStream.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliStream.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Method2_System_Void_P0_P1() { BrotliStream.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BrotliStream.Validate with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_Validate_System_Void_P0() { BrotliStream.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of BrotliStream.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Value_System_Void() { BrotliStream.Value; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Value_System_Void_P0() { BrotliStream.Value = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Count_System_Void() { BrotliStream.Count; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Count_System_Void_P0() { BrotliStream.Count = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_IsEmpty_System_Void() { BrotliStream.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_IsEmpty_System_Void_P0() { BrotliStream.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_get_Key_System_Void() { BrotliStream.Key; }
    // Purpose: Benchmark native-runtime performance of BrotliStream.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Compression_Brotli_System_IO_Compression_BrotliStream_set_Key_System_Void_P0() { BrotliStream.Key = null!; }}
