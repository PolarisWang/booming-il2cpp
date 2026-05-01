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

// Benchmark stubs for family/System.Collections.Immutable/frozen-collections
public static class FamilySystem.collections.immutableFrozenCollectionsBenchmarks
{
    // Benchmark FrozenDictionary.FrozenDictionary: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::FrozenDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object() { _ = FrozenDictionary.FrozenDictionary:(42); }
    // Benchmark FrozenDictionary.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void() { FrozenDictionary.Property; }
    // Benchmark FrozenDictionary.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0() { FrozenDictionary.Method1(null!); }
    // Benchmark FrozenDictionary.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1() { FrozenDictionary.Method2(null!, null!); }
    // Benchmark FrozenDictionary.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0() { FrozenDictionary.Validate(null!); }
    // Benchmark FrozenSet.FrozenSet: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::FrozenSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object() { _ = FrozenSet.FrozenSet:(42); }
    // Benchmark FrozenSet.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void() { FrozenSet.Property; }
    // Benchmark FrozenSet.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0() { FrozenSet.Method1(null!); }
    // Benchmark FrozenSet.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1() { FrozenSet.Method2(null!, null!); }
    // Benchmark FrozenSet.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0() { FrozenSet.Validate(null!); }
    // Benchmark FrozenDictionary.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void() { FrozenDictionary.Value; }
    // Benchmark FrozenDictionary.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0() { FrozenDictionary.Value = null!; }
    // Benchmark FrozenDictionary.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void() { FrozenDictionary.Count; }
    // Benchmark FrozenDictionary.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0() { FrozenDictionary.Count = null!; }
    // Benchmark FrozenDictionary.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void() { FrozenDictionary.IsEmpty; }
    // Benchmark FrozenDictionary.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0() { FrozenDictionary.IsEmpty = null!; }
    // Benchmark FrozenDictionary.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void() { FrozenDictionary.Key; }
    // Benchmark FrozenDictionary.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0() { FrozenDictionary.Key = null!; }
    // Benchmark FrozenSet.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void() { FrozenSet.Value; }
    // Benchmark FrozenSet.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0() { FrozenSet.Value = null!; }
    // Benchmark FrozenSet.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void() { FrozenSet.Count; }
    // Benchmark FrozenSet.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0() { FrozenSet.Count = null!; }
    // Benchmark FrozenSet.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void() { FrozenSet.IsEmpty; }
    // Benchmark FrozenSet.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0() { FrozenSet.IsEmpty = null!; }
    // Benchmark FrozenSet.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void() { FrozenSet.Key; }
    // Benchmark FrozenSet.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0() { FrozenSet.Key = null!; }}
