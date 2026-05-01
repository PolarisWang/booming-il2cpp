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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableImmutableListBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableImmutableListBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableList.ImmutableList: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::ImmutableList::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_ImmutableList_Void_System_Object() { _ = ImmutableList.ImmutableList:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableList.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Property_System_Void() { ImmutableList.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Method1_System_Void_P0() { ImmutableList.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableList.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Method2_System_Void_P0_P1() { ImmutableList.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableList.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Validate_System_Void_P0() { ImmutableList.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableList.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Value_System_Void() { ImmutableList.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Value_System_Void_P0() { ImmutableList.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Count_System_Void() { ImmutableList.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Count_System_Void_P0() { ImmutableList.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_IsEmpty_System_Void() { ImmutableList.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_IsEmpty_System_Void_P0() { ImmutableList.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_get_Key_System_Void() { ImmutableList.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableList.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableList::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableList_set_Key_System_Void_P0() { ImmutableList.Key = null!; }}
