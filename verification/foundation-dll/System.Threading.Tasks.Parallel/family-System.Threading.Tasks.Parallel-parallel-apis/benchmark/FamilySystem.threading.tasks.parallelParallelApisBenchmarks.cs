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

// Auto-generated benchmark skeletons for FamilySystem.threading.tasks.parallelParallelApisBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.threading.tasks.parallelParallelApisBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Parallel.Parallel: with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Parallel::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Parallel_Void_System_Object() { _ = Parallel.Parallel:(42); }
    // Purpose: Benchmark native-runtime performance of Parallel.get_Property with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Property_System_Void() { Parallel.Property; }
    // Purpose: Benchmark native-runtime performance of Parallel.Method1 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Method1_System_Void_P0() { Parallel.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Parallel.Method2 with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Method2_System_Void_P0_P1() { Parallel.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Parallel.Validate with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_Validate_System_Void_P0() { Parallel.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Parallel.get_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Value_System_Void() { Parallel.Value; }
    // Purpose: Benchmark native-runtime performance of Parallel.set_Value with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Value_System_Void_P0() { Parallel.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Parallel.get_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Count_System_Void() { Parallel.Count; }
    // Purpose: Benchmark native-runtime performance of Parallel.set_Count with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Count_System_Void_P0() { Parallel.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Parallel.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_IsEmpty_System_Void() { Parallel.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Parallel.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_IsEmpty_System_Void_P0() { Parallel.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Parallel.get_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_get_Key_System_Void() { Parallel.Key; }
    // Purpose: Benchmark native-runtime performance of Parallel.set_Key with typical input
    [BenchmarkSubjectId("System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Threading_Tasks_Parallel_System_Threading_Tasks_Parallel_set_Key_System_Void_P0() { Parallel.Key = null!; }}
