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

// Auto-generated benchmark skeletons for FamilySystem.threading.tasks.parallelLoopResultBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.threading.tasks.parallelLoopResultBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.ParallelLoopResult: with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::ParallelLoopResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_ParallelLoopResult_Void_System_Object() { _ = ParallelLoopResult.ParallelLoopResult:(42); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.get_Property with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Property_System_Void() { ParallelLoopResult.Property; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.Method1 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method1_System_Void_P0() { ParallelLoopResult.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.Method2 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method2_System_Void_P0_P1() { ParallelLoopResult.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.Validate with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Validate_System_Void_P0() { ParallelLoopResult.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.get_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Value_System_Void() { ParallelLoopResult.Value; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.set_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Value_System_Void_P0() { ParallelLoopResult.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.get_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Count_System_Void() { ParallelLoopResult.Count; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.set_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Count_System_Void_P0() { ParallelLoopResult.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_IsEmpty_System_Void() { ParallelLoopResult.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_IsEmpty_System_Void_P0() { ParallelLoopResult.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.get_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Key_System_Void() { ParallelLoopResult.Key; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopResult.set_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Key_System_Void_P0() { ParallelLoopResult.Key = null!; }}
