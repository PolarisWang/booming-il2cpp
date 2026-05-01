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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableImmutableSortedDictionaryBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableImmutableSortedDictionaryBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.ImmutableSortedDictionary: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::ImmutableSortedDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_ImmutableSortedDictionary_Void_Syste() { _ = ImmutableSortedDictionary.ImmutableSortedDictionary:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Property_System_Void() { ImmutableSortedDictionary.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method1_System_Void_P0() { ImmutableSortedDictionary.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Method2_System_Void_P0_P1() { ImmutableSortedDictionary.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_Validate_System_Void_P0() { ImmutableSortedDictionary.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Value_System_Void() { ImmutableSortedDictionary.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Value_System_Void_P0() { ImmutableSortedDictionary.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Count_System_Void() { ImmutableSortedDictionary.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Count_System_Void_P0() { ImmutableSortedDictionary.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_IsEmpty_System_Void() { ImmutableSortedDictionary.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_IsEmpty_System_Void_P0() { ImmutableSortedDictionary.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_get_Key_System_Void() { ImmutableSortedDictionary.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableSortedDictionary.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableSortedDictionary_set_Key_System_Void_P0() { ImmutableSortedDictionary.Key = null!; }}
