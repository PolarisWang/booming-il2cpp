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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableImmutableInterlockedBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableImmutableInterlockedBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.ImmutableInterlocked: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::ImmutableInterlocked::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_ImmutableInterlocked_Void_System_Object() { _ = ImmutableInterlocked.ImmutableInterlocked:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Property_System_Void() { ImmutableInterlocked.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method1_System_Void_P0() { ImmutableInterlocked.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method2_System_Void_P0_P1() { ImmutableInterlocked.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Validate_System_Void_P0() { ImmutableInterlocked.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Value_System_Void() { ImmutableInterlocked.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Value_System_Void_P0() { ImmutableInterlocked.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Count_System_Void() { ImmutableInterlocked.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Count_System_Void_P0() { ImmutableInterlocked.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_IsEmpty_System_Void() { ImmutableInterlocked.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_IsEmpty_System_Void_P0() { ImmutableInterlocked.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Key_System_Void() { ImmutableInterlocked.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableInterlocked.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Key_System_Void_P0() { ImmutableInterlocked.Key = null!; }}
