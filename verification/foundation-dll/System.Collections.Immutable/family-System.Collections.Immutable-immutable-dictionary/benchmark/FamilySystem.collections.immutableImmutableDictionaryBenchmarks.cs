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

// Benchmark stubs for family/System.Collections.Immutable/immutable-dictionary
public static class FamilySystem.collections.immutableImmutableDictionaryBenchmarks
{
    // Benchmark ImmutableDictionary.ImmutableDictionary: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::ImmutableDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_ImmutableDictionary_Void_System_Object() { _ = ImmutableDictionary.ImmutableDictionary:(42); }
    // Benchmark ImmutableDictionary.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Property_System_Void() { ImmutableDictionary.Property; }
    // Benchmark ImmutableDictionary.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method1_System_Void_P0() { ImmutableDictionary.Method1(null!); }
    // Benchmark ImmutableDictionary.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Method2_System_Void_P0_P1() { ImmutableDictionary.Method2(null!, null!); }
    // Benchmark ImmutableDictionary.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Validate_System_Void_P0() { ImmutableDictionary.Validate(null!); }
    // Benchmark ImmutableDictionary.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Value_System_Void() { ImmutableDictionary.Value; }
    // Benchmark ImmutableDictionary.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Value_System_Void_P0() { ImmutableDictionary.Value = null!; }
    // Benchmark ImmutableDictionary.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Count_System_Void() { ImmutableDictionary.Count; }
    // Benchmark ImmutableDictionary.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Count_System_Void_P0() { ImmutableDictionary.Count = null!; }
    // Benchmark ImmutableDictionary.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_IsEmpty_System_Void() { ImmutableDictionary.IsEmpty; }
    // Benchmark ImmutableDictionary.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_IsEmpty_System_Void_P0() { ImmutableDictionary.IsEmpty = null!; }
    // Benchmark ImmutableDictionary.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_get_Key_System_Void() { ImmutableDictionary.Key; }
    // Benchmark ImmutableDictionary.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_set_Key_System_Void_P0() { ImmutableDictionary.Key = null!; }}
