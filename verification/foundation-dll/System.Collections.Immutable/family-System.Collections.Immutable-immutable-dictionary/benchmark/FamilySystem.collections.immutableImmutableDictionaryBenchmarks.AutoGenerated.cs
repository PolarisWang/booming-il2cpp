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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableImmutableDictionaryBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableImmutableDictionaryBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.ImmutableDictionary: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::ImmutableDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_ImmutableDictionary_Void_System_Object() { _ = ImmutableDictionary.ImmutableDictionary:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Property_System_Void() { ImmutableDictionary.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method1_System_Void_P0() { ImmutableDictionary.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method2_System_Void_P0_P1() { ImmutableDictionary.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Validate_System_Void_P0() { ImmutableDictionary.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Value_System_Void() { ImmutableDictionary.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Value_System_Void_P0() { ImmutableDictionary.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Count_System_Void() { ImmutableDictionary.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Count_System_Void_P0() { ImmutableDictionary.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_IsEmpty_System_Void() { ImmutableDictionary.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_IsEmpty_System_Void_P0() { ImmutableDictionary.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Key_System_Void() { ImmutableDictionary.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableDictionary.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Key_System_Void_P0() { ImmutableDictionary.Key = null!; }}
