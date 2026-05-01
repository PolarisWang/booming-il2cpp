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

// Auto-generated benchmark skeletons for FamilySystem.linqPartitioningBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.linqPartitioningBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Enumerable.Enumerable: with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::Enumerable::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_Enumerable_Void_System_Object() { _ = Enumerable.Enumerable:(42); }
    // Purpose: Benchmark native-runtime performance of Enumerable.get_Property with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_get_Property_System_Void() { Enumerable.Property; }
    // Purpose: Benchmark native-runtime performance of Enumerable.Method1 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_Method1_System_Void_P0() { Enumerable.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Enumerable.Method2 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_Method2_System_Void_P0_P1() { Enumerable.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Enumerable.Validate with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_Validate_System_Void_P0() { Enumerable.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of IGrouping.IGrouping: with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::IGrouping::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_IGrouping_Void_System_Object() { _ = IGrouping.IGrouping:(42); }
    // Purpose: Benchmark native-runtime performance of IGrouping.get_Property with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_get_Property_System_Void() { IGrouping.Property; }
    // Purpose: Benchmark native-runtime performance of IGrouping.Method1 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_Method1_System_Void_P0() { IGrouping.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of IGrouping.Method2 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_Method2_System_Void_P0_P1() { IGrouping.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IGrouping.Validate with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_Validate_System_Void_P0() { IGrouping.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ILookup.ILookup: with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::ILookup::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_ILookup_Void_System_Object() { _ = ILookup.ILookup:(42); }
    // Purpose: Benchmark native-runtime performance of ILookup.get_Property with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_get_Property_System_Void() { ILookup.Property; }
    // Purpose: Benchmark native-runtime performance of ILookup.Method1 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_Method1_System_Void_P0() { ILookup.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ILookup.Method2 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_Method2_System_Void_P0_P1() { ILookup.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ILookup.Validate with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_Validate_System_Void_P0() { ILookup.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.IOrderedEnumerable: with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::IOrderedEnumerable::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_IOrderedEnumerable_Void_System_Object() { _ = IOrderedEnumerable.IOrderedEnumerable:(42); }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.get_Property with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_get_Property_System_Void() { IOrderedEnumerable.Property; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.Method1 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_Method1_System_Void_P0() { IOrderedEnumerable.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.Method2 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_Method2_System_Void_P0_P1() { IOrderedEnumerable.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.Validate with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_Validate_System_Void_P0() { IOrderedEnumerable.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Lookup.Lookup: with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::Lookup::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_Lookup_Void_System_Object() { _ = Lookup.Lookup:(42); }
    // Purpose: Benchmark native-runtime performance of Lookup.get_Property with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_get_Property_System_Void() { Lookup.Property; }
    // Purpose: Benchmark native-runtime performance of Lookup.Method1 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_Method1_System_Void_P0() { Lookup.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Lookup.Method2 with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_Method2_System_Void_P0_P1() { Lookup.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Lookup.Validate with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_Validate_System_Void_P0() { Lookup.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Enumerable.get_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_get_Value_System_Void() { Enumerable.Value; }
    // Purpose: Benchmark native-runtime performance of Enumerable.set_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_set_Value_System_Void_P0() { Enumerable.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Enumerable.get_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_get_Count_System_Void() { Enumerable.Count; }
    // Purpose: Benchmark native-runtime performance of Enumerable.set_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_set_Count_System_Void_P0() { Enumerable.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Enumerable.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_get_IsEmpty_System_Void() { Enumerable.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Enumerable.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_set_IsEmpty_System_Void_P0() { Enumerable.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Enumerable.get_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_get_Key_System_Void() { Enumerable.Key; }
    // Purpose: Benchmark native-runtime performance of Enumerable.set_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Enumerable::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Enumerable_set_Key_System_Void_P0() { Enumerable.Key = null!; }
    // Purpose: Benchmark native-runtime performance of IGrouping.get_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_get_Value_System_Void() { IGrouping.Value; }
    // Purpose: Benchmark native-runtime performance of IGrouping.set_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_set_Value_System_Void_P0() { IGrouping.Value = null!; }
    // Purpose: Benchmark native-runtime performance of IGrouping.get_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_get_Count_System_Void() { IGrouping.Count; }
    // Purpose: Benchmark native-runtime performance of IGrouping.set_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_set_Count_System_Void_P0() { IGrouping.Count = null!; }
    // Purpose: Benchmark native-runtime performance of IGrouping.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_get_IsEmpty_System_Void() { IGrouping.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of IGrouping.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_set_IsEmpty_System_Void_P0() { IGrouping.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of IGrouping.get_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_get_Key_System_Void() { IGrouping.Key; }
    // Purpose: Benchmark native-runtime performance of IGrouping.set_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IGrouping::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IGrouping_set_Key_System_Void_P0() { IGrouping.Key = null!; }
    // Purpose: Benchmark native-runtime performance of ILookup.get_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_get_Value_System_Void() { ILookup.Value; }
    // Purpose: Benchmark native-runtime performance of ILookup.set_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_set_Value_System_Void_P0() { ILookup.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ILookup.get_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_get_Count_System_Void() { ILookup.Count; }
    // Purpose: Benchmark native-runtime performance of ILookup.set_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_set_Count_System_Void_P0() { ILookup.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ILookup.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_get_IsEmpty_System_Void() { ILookup.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ILookup.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_set_IsEmpty_System_Void_P0() { ILookup.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ILookup.get_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_get_Key_System_Void() { ILookup.Key; }
    // Purpose: Benchmark native-runtime performance of ILookup.set_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.ILookup::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_ILookup_set_Key_System_Void_P0() { ILookup.Key = null!; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.get_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_get_Value_System_Void() { IOrderedEnumerable.Value; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.set_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_set_Value_System_Void_P0() { IOrderedEnumerable.Value = null!; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.get_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_get_Count_System_Void() { IOrderedEnumerable.Count; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.set_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_set_Count_System_Void_P0() { IOrderedEnumerable.Count = null!; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_get_IsEmpty_System_Void() { IOrderedEnumerable.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_set_IsEmpty_System_Void_P0() { IOrderedEnumerable.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.get_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_get_Key_System_Void() { IOrderedEnumerable.Key; }
    // Purpose: Benchmark native-runtime performance of IOrderedEnumerable.set_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.IOrderedEnumerable::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_IOrderedEnumerable_set_Key_System_Void_P0() { IOrderedEnumerable.Key = null!; }
    // Purpose: Benchmark native-runtime performance of Lookup.get_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_get_Value_System_Void() { Lookup.Value; }
    // Purpose: Benchmark native-runtime performance of Lookup.set_Value with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_set_Value_System_Void_P0() { Lookup.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Lookup.get_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_get_Count_System_Void() { Lookup.Count; }
    // Purpose: Benchmark native-runtime performance of Lookup.set_Count with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_set_Count_System_Void_P0() { Lookup.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Lookup.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_get_IsEmpty_System_Void() { Lookup.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Lookup.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_set_IsEmpty_System_Void_P0() { Lookup.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Lookup.get_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_get_Key_System_Void() { Lookup.Key; }
    // Purpose: Benchmark native-runtime performance of Lookup.set_Key with typical input
    [BenchmarkSubjectId("System.Linq/System.Linq.Lookup::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Linq_System_Linq_Lookup_set_Key_System_Void_P0() { Lookup.Key = null!; }}
