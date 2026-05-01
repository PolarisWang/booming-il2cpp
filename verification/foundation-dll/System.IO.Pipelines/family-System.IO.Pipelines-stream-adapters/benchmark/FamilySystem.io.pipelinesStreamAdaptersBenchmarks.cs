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

// Auto-generated benchmark skeletons for FamilySystem.io.pipelinesStreamAdaptersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.pipelinesStreamAdaptersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.StreamPipeExtensions: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::StreamPipeExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_StreamPipeExtensions_Void_System_Object() { _ = StreamPipeExtensions.StreamPipeExtensions:(42); }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Property_System_Void() { StreamPipeExtensions.Property; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method1_System_Void_P0() { StreamPipeExtensions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method2_System_Void_P0_P1() { StreamPipeExtensions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Validate_System_Void_P0() { StreamPipeExtensions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Value_System_Void() { StreamPipeExtensions.Value; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Value_System_Void_P0() { StreamPipeExtensions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Count_System_Void() { StreamPipeExtensions.Count; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Count_System_Void_P0() { StreamPipeExtensions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_IsEmpty_System_Void() { StreamPipeExtensions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_IsEmpty_System_Void_P0() { StreamPipeExtensions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Key_System_Void() { StreamPipeExtensions.Key; }
    // Purpose: Benchmark native-runtime performance of StreamPipeExtensions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Key_System_Void_P0() { StreamPipeExtensions.Key = null!; }}
