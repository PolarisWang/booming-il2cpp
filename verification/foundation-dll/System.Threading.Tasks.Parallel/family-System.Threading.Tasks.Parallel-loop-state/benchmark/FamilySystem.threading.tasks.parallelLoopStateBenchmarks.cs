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

// Auto-generated benchmark skeletons for FamilySystem.threading.tasks.parallelLoopStateBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.threading.tasks.parallelLoopStateBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.ParallelLoopState: with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::ParallelLoopState::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_ParallelLoopState_Void_System_Object() { _ = ParallelLoopState.ParallelLoopState:(42); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.get_Property with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Property_System_Void() { ParallelLoopState.Property; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.Method1 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method1_System_Void_P0() { ParallelLoopState.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.Method2 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method2_System_Void_P0_P1() { ParallelLoopState.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.Validate with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Validate_System_Void_P0() { ParallelLoopState.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.get_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Value_System_Void() { ParallelLoopState.Value; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.set_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Value_System_Void_P0() { ParallelLoopState.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.get_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Count_System_Void() { ParallelLoopState.Count; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.set_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Count_System_Void_P0() { ParallelLoopState.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_IsEmpty_System_Void() { ParallelLoopState.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_IsEmpty_System_Void_P0() { ParallelLoopState.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.get_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Key_System_Void() { ParallelLoopState.Key; }
    // Purpose: Benchmark native-runtime performance of ParallelLoopState.set_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Key_System_Void_P0() { ParallelLoopState.Key = null!; }}
