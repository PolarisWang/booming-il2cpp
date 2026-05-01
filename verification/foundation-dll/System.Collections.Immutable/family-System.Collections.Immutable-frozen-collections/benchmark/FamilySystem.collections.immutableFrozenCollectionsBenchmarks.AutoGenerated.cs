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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableFrozenCollectionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableFrozenCollectionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.FrozenDictionary: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::FrozenDictionary::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object() { _ = FrozenDictionary.FrozenDictionary:(42); }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void() { FrozenDictionary.Property; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0() { FrozenDictionary.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1() { FrozenDictionary.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0() { FrozenDictionary.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of FrozenSet.FrozenSet: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::FrozenSet::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object() { _ = FrozenSet.FrozenSet:(42); }
    // Purpose: Benchmark native-runtime performance of FrozenSet.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void() { FrozenSet.Property; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0() { FrozenSet.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of FrozenSet.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1() { FrozenSet.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FrozenSet.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0() { FrozenSet.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void() { FrozenDictionary.Value; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0() { FrozenDictionary.Value = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void() { FrozenDictionary.Count; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0() { FrozenDictionary.Count = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void() { FrozenDictionary.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0() { FrozenDictionary.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void() { FrozenDictionary.Key; }
    // Purpose: Benchmark native-runtime performance of FrozenDictionary.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0() { FrozenDictionary.Key = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void() { FrozenSet.Value; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0() { FrozenSet.Value = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void() { FrozenSet.Count; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0() { FrozenSet.Count = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void() { FrozenSet.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0() { FrozenSet.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void() { FrozenSet.Key; }
    // Purpose: Benchmark native-runtime performance of FrozenSet.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0() { FrozenSet.Key = null!; }}
