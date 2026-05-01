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

// Benchmark stubs for family/System.Collections.Immutable/extensions-marshal
public static class FamilySystem.collections.immutableExtensionsMarshalBenchmarks
{
    // Benchmark ImmutableArrayExtensions.ImmutableArrayExtensions: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::ImmutableArrayExtensions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_ImmutableArrayExtensions_Void_System_Object() { _ = ImmutableArrayExtensions.ImmutableArrayExtensions:(42); }
    // Benchmark ImmutableArrayExtensions.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Property_System_Void() { ImmutableArrayExtensions.Property; }
    // Benchmark ImmutableArrayExtensions.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method1_System_Void_P0() { ImmutableArrayExtensions.Method1(null!); }
    // Benchmark ImmutableArrayExtensions.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method2_System_Void_P0_P1() { ImmutableArrayExtensions.Method2(null!, null!); }
    // Benchmark ImmutableArrayExtensions.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Validate_System_Void_P0() { ImmutableArrayExtensions.Validate(null!); }
    // Benchmark ImmutableCollectionsMarshal.ImmutableCollectionsMarshal: native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::ImmutableCollectionsMarshal::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_ImmutableCollectionsMarshal_Void() { _ = ImmutableCollectionsMarshal.ImmutableCollectionsMarshal:(42); }
    // Benchmark ImmutableCollectionsMarshal.get_Property native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Property_System_Void() { ImmutableCollectionsMarshal.Property; }
    // Benchmark ImmutableCollectionsMarshal.Method1 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method1_System_Void_P0() { ImmutableCollectionsMarshal.Method1(null!); }
    // Benchmark ImmutableCollectionsMarshal.Method2 native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method2_System_Void_P0_P1() { ImmutableCollectionsMarshal.Method2(null!, null!); }
    // Benchmark ImmutableCollectionsMarshal.Validate native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Validate_System_Void_P0() { ImmutableCollectionsMarshal.Validate(null!); }
    // Benchmark ImmutableArrayExtensions.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Value_System_Void() { ImmutableArrayExtensions.Value; }
    // Benchmark ImmutableArrayExtensions.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Value_System_Void_P0() { ImmutableArrayExtensions.Value = null!; }
    // Benchmark ImmutableArrayExtensions.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Count_System_Void() { ImmutableArrayExtensions.Count; }
    // Benchmark ImmutableArrayExtensions.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Count_System_Void_P0() { ImmutableArrayExtensions.Count = null!; }
    // Benchmark ImmutableArrayExtensions.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_IsEmpty_System_Void() { ImmutableArrayExtensions.IsEmpty; }
    // Benchmark ImmutableArrayExtensions.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_IsEmpty_System_Void_P0() { ImmutableArrayExtensions.IsEmpty = null!; }
    // Benchmark ImmutableArrayExtensions.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Key_System_Void() { ImmutableArrayExtensions.Key; }
    // Benchmark ImmutableArrayExtensions.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Key_System_Void_P0() { ImmutableArrayExtensions.Key = null!; }
    // Benchmark ImmutableCollectionsMarshal.get_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Value_System_Void() { ImmutableCollectionsMarshal.Value; }
    // Benchmark ImmutableCollectionsMarshal.set_Value native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Value_System_Void_P0() { ImmutableCollectionsMarshal.Value = null!; }
    // Benchmark ImmutableCollectionsMarshal.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Count_System_Void() { ImmutableCollectionsMarshal.Count; }
    // Benchmark ImmutableCollectionsMarshal.set_Count native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Count_System_Void_P0() { ImmutableCollectionsMarshal.Count = null!; }
    // Benchmark ImmutableCollectionsMarshal.get_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_IsEmpty_System_Void() { ImmutableCollectionsMarshal.IsEmpty; }
    // Benchmark ImmutableCollectionsMarshal.set_IsEmpty native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_IsEmpty_System_Void_P0() { ImmutableCollectionsMarshal.IsEmpty = null!; }
    // Benchmark ImmutableCollectionsMarshal.get_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Key_System_Void() { ImmutableCollectionsMarshal.Key; }
    // Benchmark ImmutableCollectionsMarshal.set_Key native-runtime throughput
    [BenchmarkSubjectId("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Key_System_Void_P0() { ImmutableCollectionsMarshal.Key = null!; }}
