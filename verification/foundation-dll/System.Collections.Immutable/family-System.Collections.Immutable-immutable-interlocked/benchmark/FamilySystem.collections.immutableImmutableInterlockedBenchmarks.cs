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

// Benchmark stubs for family/System.Collections.Immutable/immutable-interlocked
public static class FamilySystem.collections.immutableImmutableInterlockedBenchmarks
{
    // Benchmark ImmutableInterlocked.ImmutableInterlocked: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::ImmutableInterlocked::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_ImmutableInterlocked_Void_System_Object() { _ = ImmutableInterlocked.ImmutableInterlocked:(42); }
    // Benchmark ImmutableInterlocked.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Property_System_Void() { ImmutableInterlocked.Property; }
    // Benchmark ImmutableInterlocked.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method1_System_Void_P0() { ImmutableInterlocked.Method1(null!); }
    // Benchmark ImmutableInterlocked.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method2_System_Void_P0_P1() { ImmutableInterlocked.Method2(null!, null!); }
    // Benchmark ImmutableInterlocked.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Validate_System_Void_P0() { ImmutableInterlocked.Validate(null!); }
    // Benchmark ImmutableInterlocked.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Value_System_Void() { ImmutableInterlocked.Value; }
    // Benchmark ImmutableInterlocked.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Value_System_Void_P0() { ImmutableInterlocked.Value = null!; }
    // Benchmark ImmutableInterlocked.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Count_System_Void() { ImmutableInterlocked.Count; }
    // Benchmark ImmutableInterlocked.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Count_System_Void_P0() { ImmutableInterlocked.Count = null!; }
    // Benchmark ImmutableInterlocked.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_IsEmpty_System_Void() { ImmutableInterlocked.IsEmpty; }
    // Benchmark ImmutableInterlocked.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_IsEmpty_System_Void_P0() { ImmutableInterlocked.IsEmpty = null!; }
    // Benchmark ImmutableInterlocked.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Key_System_Void() { ImmutableInterlocked.Key; }
    // Benchmark ImmutableInterlocked.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Key_System_Void_P0() { ImmutableInterlocked.Key = null!; }}
