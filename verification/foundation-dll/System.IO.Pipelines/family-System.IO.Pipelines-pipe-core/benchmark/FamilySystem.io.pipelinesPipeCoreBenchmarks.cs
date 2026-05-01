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

// Auto-generated benchmark skeletons for FamilySystem.io.pipelinesPipeCoreBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.pipelinesPipeCoreBenchmarks
{
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.IDuplexPipe: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::IDuplexPipe::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_IDuplexPipe_Void_System_Object() { _ = IDuplexPipe.IDuplexPipe:(42); }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Property_System_Void() { IDuplexPipe.Property; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method1_System_Void_P0() { IDuplexPipe.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method2_System_Void_P0_P1() { IDuplexPipe.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Validate_System_Void_P0() { IDuplexPipe.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Pipe.Pipe: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Pipe::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_Pipe_Void_System_Object() { _ = Pipe.Pipe:(42); }
    // Purpose: Benchmark native-runtime performance of Pipe.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Property_System_Void() { Pipe.Property; }
    // Purpose: Benchmark native-runtime performance of Pipe.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_Method1_System_Void_P0() { Pipe.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Pipe.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_Method2_System_Void_P0_P1() { Pipe.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Pipe.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_Validate_System_Void_P0() { Pipe.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Value_System_Void() { IDuplexPipe.Value; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Value_System_Void_P0() { IDuplexPipe.Value = null!; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Count_System_Void() { IDuplexPipe.Count; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Count_System_Void_P0() { IDuplexPipe.Count = null!; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_IsEmpty_System_Void() { IDuplexPipe.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_IsEmpty_System_Void_P0() { IDuplexPipe.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Key_System_Void() { IDuplexPipe.Key; }
    // Purpose: Benchmark native-runtime performance of IDuplexPipe.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Key_System_Void_P0() { IDuplexPipe.Key = null!; }
    // Purpose: Benchmark native-runtime performance of Pipe.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Value_System_Void() { Pipe.Value; }
    // Purpose: Benchmark native-runtime performance of Pipe.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Value_System_Void_P0() { Pipe.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Pipe.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Count_System_Void() { Pipe.Count; }
    // Purpose: Benchmark native-runtime performance of Pipe.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Count_System_Void_P0() { Pipe.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Pipe.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_IsEmpty_System_Void() { Pipe.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Pipe.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_IsEmpty_System_Void_P0() { Pipe.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Pipe.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Key_System_Void() { Pipe.Key; }
    // Purpose: Benchmark native-runtime performance of Pipe.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Key_System_Void_P0() { Pipe.Key = null!; }}
