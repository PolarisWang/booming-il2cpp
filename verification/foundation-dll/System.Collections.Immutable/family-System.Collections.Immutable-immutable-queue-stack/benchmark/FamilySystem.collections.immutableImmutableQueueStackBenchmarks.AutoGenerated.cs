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

// Auto-generated benchmark skeletons for FamilySystem.collections.immutableImmutableQueueStackBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.collections.immutableImmutableQueueStackBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.ImmutableQueue: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::ImmutableQueue::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_ImmutableQueue_Void_System_Object() { _ = ImmutableQueue.ImmutableQueue:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Property_System_Void() { ImmutableQueue.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method1_System_Void_P0() { ImmutableQueue.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method2_System_Void_P0_P1() { ImmutableQueue.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Validate_System_Void_P0() { ImmutableQueue.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.ImmutableStack: with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::ImmutableStack::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_ImmutableStack_Void_System_Object() { _ = ImmutableStack.ImmutableStack:(42); }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.get_Property with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Property_System_Void() { ImmutableStack.Property; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.Method1 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method1_System_Void_P0() { ImmutableStack.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.Method2 with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method2_System_Void_P0_P1() { ImmutableStack.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.Validate with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Validate_System_Void_P0() { ImmutableStack.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Value_System_Void() { ImmutableQueue.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Value_System_Void_P0() { ImmutableQueue.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Count_System_Void() { ImmutableQueue.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Count_System_Void_P0() { ImmutableQueue.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_IsEmpty_System_Void() { ImmutableQueue.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_IsEmpty_System_Void_P0() { ImmutableQueue.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Key_System_Void() { ImmutableQueue.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableQueue.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Key_System_Void_P0() { ImmutableQueue.Key = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.get_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Value_System_Void() { ImmutableStack.Value; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.set_Value with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Value_System_Void_P0() { ImmutableStack.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.get_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Count_System_Void() { ImmutableStack.Count; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.set_Count with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Count_System_Void_P0() { ImmutableStack.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_IsEmpty_System_Void() { ImmutableStack.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_IsEmpty_System_Void_P0() { ImmutableStack.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.get_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Key_System_Void() { ImmutableStack.Key; }
    // Purpose: Benchmark native-runtime performance of ImmutableStack.set_Key with typical input
    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Key_System_Void_P0() { ImmutableStack.Key = null!; }}
