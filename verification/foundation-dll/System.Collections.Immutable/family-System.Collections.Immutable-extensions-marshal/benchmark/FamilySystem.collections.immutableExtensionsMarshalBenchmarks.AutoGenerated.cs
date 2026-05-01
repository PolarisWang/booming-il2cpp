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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableExtensionsMarshalBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableExtensionsMarshalBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.ImmutableArrayExtensions: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::ImmutableArrayExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_ImmutableArrayExtensions_Void_System_Object() { _ = ImmutableArrayExtensions.ImmutableArrayExtensions:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Property_System_Void() { ImmutableArrayExtensions.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method1_System_Void_P0() { ImmutableArrayExtensions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method2_System_Void_P0_P1() { ImmutableArrayExtensions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Validate_System_Void_P0() { ImmutableArrayExtensions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.ImmutableCollectionsMarshal: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::ImmutableCollectionsMarshal::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_ImmutableCollectionsMarshal_Void() { _ = ImmutableCollectionsMarshal.ImmutableCollectionsMarshal:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Property_System_Void() { ImmutableCollectionsMarshal.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method1_System_Void_P0() { ImmutableCollectionsMarshal.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method2_System_Void_P0_P1() { ImmutableCollectionsMarshal.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Validate_System_Void_P0() { ImmutableCollectionsMarshal.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Value_System_Void() { ImmutableArrayExtensions.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Value_System_Void_P0() { ImmutableArrayExtensions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Count_System_Void() { ImmutableArrayExtensions.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Count_System_Void_P0() { ImmutableArrayExtensions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_IsEmpty_System_Void() { ImmutableArrayExtensions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_IsEmpty_System_Void_P0() { ImmutableArrayExtensions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Key_System_Void() { ImmutableArrayExtensions.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableArrayExtensions.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Key_System_Void_P0() { ImmutableArrayExtensions.Key = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Value_System_Void() { ImmutableCollectionsMarshal.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Value_System_Void_P0() { ImmutableCollectionsMarshal.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Count_System_Void() { ImmutableCollectionsMarshal.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Count_System_Void_P0() { ImmutableCollectionsMarshal.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_IsEmpty_System_Void() { ImmutableCollectionsMarshal.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_IsEmpty_System_Void_P0() { ImmutableCollectionsMarshal.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Key_System_Void() { ImmutableCollectionsMarshal.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableCollectionsMarshal.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Key_System_Void_P0() { ImmutableCollectionsMarshal.Key = null!; }}
