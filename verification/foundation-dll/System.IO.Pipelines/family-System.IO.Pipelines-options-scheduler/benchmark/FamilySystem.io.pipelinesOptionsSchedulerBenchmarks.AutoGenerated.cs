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

// Auto-generated benchmark skeletons for FamilySystem.io.pipelinesOptionsSchedulerBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.pipelinesOptionsSchedulerBenchmarks
{
    // Purpose: Benchmark native-runtime performance of PipeOptions.PipeOptions: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::PipeOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_PipeOptions_Void_System_Object() { _ = PipeOptions.PipeOptions:(42); }
    // Purpose: Benchmark native-runtime performance of PipeOptions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Property_System_Void() { PipeOptions.Property; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Method1_System_Void_P0() { PipeOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of PipeOptions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Method2_System_Void_P0_P1() { PipeOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PipeOptions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Validate_System_Void_P0() { PipeOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.PipeScheduler: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::PipeScheduler::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_PipeScheduler_Void_System_Object() { _ = PipeScheduler.PipeScheduler:(42); }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Property_System_Void() { PipeScheduler.Property; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Method1_System_Void_P0() { PipeScheduler.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Method2_System_Void_P0_P1() { PipeScheduler.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Validate_System_Void_P0() { PipeScheduler.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.StreamPipeReaderOptions: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::StreamPipeReaderOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_StreamPipeReaderOptions_Void_System_Object() { _ = StreamPipeReaderOptions.StreamPipeReaderOptions:(42); }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Property_System_Void() { StreamPipeReaderOptions.Property; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Method1_System_Void_P0() { StreamPipeReaderOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Method2_System_Void_P0_P1() { StreamPipeReaderOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Validate_System_Void_P0() { StreamPipeReaderOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.StreamPipeWriterOptions: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::StreamPipeWriterOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_StreamPipeWriterOptions_Void_System_Object() { _ = StreamPipeWriterOptions.StreamPipeWriterOptions:(42); }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Property_System_Void() { StreamPipeWriterOptions.Property; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Method1_System_Void_P0() { StreamPipeWriterOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Method2_System_Void_P0_P1() { StreamPipeWriterOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Validate_System_Void_P0() { StreamPipeWriterOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of PipeOptions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Value_System_Void() { PipeOptions.Value; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Value_System_Void_P0() { PipeOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Count_System_Void() { PipeOptions.Count; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Count_System_Void_P0() { PipeOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_IsEmpty_System_Void() { PipeOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_IsEmpty_System_Void_P0() { PipeOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Key_System_Void() { PipeOptions.Key; }
    // Purpose: Benchmark native-runtime performance of PipeOptions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Key_System_Void_P0() { PipeOptions.Key = null!; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Value_System_Void() { PipeScheduler.Value; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Value_System_Void_P0() { PipeScheduler.Value = null!; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Count_System_Void() { PipeScheduler.Count; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Count_System_Void_P0() { PipeScheduler.Count = null!; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_IsEmpty_System_Void() { PipeScheduler.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_IsEmpty_System_Void_P0() { PipeScheduler.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Key_System_Void() { PipeScheduler.Key; }
    // Purpose: Benchmark native-runtime performance of PipeScheduler.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Key_System_Void_P0() { PipeScheduler.Key = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Value_System_Void() { StreamPipeReaderOptions.Value; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Value_System_Void_P0() { StreamPipeReaderOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Count_System_Void() { StreamPipeReaderOptions.Count; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Count_System_Void_P0() { StreamPipeReaderOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_IsEmpty_System_Void() { StreamPipeReaderOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_IsEmpty_System_Void_P0() { StreamPipeReaderOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Key_System_Void() { StreamPipeReaderOptions.Key; }
    // Purpose: Benchmark native-runtime performance of StreamPipeReaderOptions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Key_System_Void_P0() { StreamPipeReaderOptions.Key = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Value_System_Void() { StreamPipeWriterOptions.Value; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Value_System_Void_P0() { StreamPipeWriterOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Count_System_Void() { StreamPipeWriterOptions.Count; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Count_System_Void_P0() { StreamPipeWriterOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_IsEmpty_System_Void() { StreamPipeWriterOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_IsEmpty_System_Void_P0() { StreamPipeWriterOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Key_System_Void() { StreamPipeWriterOptions.Key; }
    // Purpose: Benchmark native-runtime performance of StreamPipeWriterOptions.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Key_System_Void_P0() { StreamPipeWriterOptions.Key = null!; }}
