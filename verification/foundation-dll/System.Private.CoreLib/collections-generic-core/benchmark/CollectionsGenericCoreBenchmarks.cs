using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/collections/generic-core
public static class CollectionsGenericCoreBenchmarks
{
    // Benchmark List.Add native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }
    // Benchmark List.Clear native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }
    // Benchmark List.Contains native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }
    // Benchmark List.IndexOf native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }
    // Benchmark List.Remove native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }
    // Benchmark List.RemoveAt native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { new System.Collections.Generic.List<int>{1,2,3}.RemoveAt(1); }
    // Benchmark List.Sort native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }
    // Benchmark List.ToArray native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }
    // Benchmark Dictionary.Add native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }
    // Benchmark Dictionary.get_Count native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }
    // Benchmark Dictionary.TryGetValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }
    // Benchmark Dictionary.ContainsKey native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }
    // Benchmark Dictionary.Remove native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }
    // Benchmark HashSet.Add native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }
    // Benchmark HashSet.Contains native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }
    // Benchmark HashSet.Remove native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }}
