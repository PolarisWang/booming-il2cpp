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

// Auto-generated benchmark skeletons for FamilySystem.threading.tasks.parallelParallelOptionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.threading.tasks.parallelParallelOptionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ParallelOptions.ParallelOptions: with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::ParallelOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_ParallelOptions_Void_System_Object() { _ = ParallelOptions.ParallelOptions:(42); }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.get_Property with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Property_System_Void() { ParallelOptions.Property; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.Method1 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Method1_System_Void_P0() { ParallelOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.Method2 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Method2_System_Void_P0_P1() { ParallelOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.Validate with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_Validate_System_Void_P0() { ParallelOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.get_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Value_System_Void() { ParallelOptions.Value; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.set_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Value_System_Void_P0() { ParallelOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.get_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Count_System_Void() { ParallelOptions.Count; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.set_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Count_System_Void_P0() { ParallelOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_IsEmpty_System_Void() { ParallelOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_IsEmpty_System_Void_P0() { ParallelOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.get_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_get_Key_System_Void() { ParallelOptions.Key; }
    // Purpose: Benchmark native-runtime performance of ParallelOptions.set_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelOptions_set_Key_System_Void_P0() { ParallelOptions.Key = null!; }}
