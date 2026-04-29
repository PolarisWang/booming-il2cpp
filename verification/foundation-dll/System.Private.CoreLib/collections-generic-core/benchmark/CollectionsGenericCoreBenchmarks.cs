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

// Auto-generated benchmark skeletons for CollectionsGenericCoreBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class CollectionsGenericCoreBenchmarks
{
    // Purpose: Benchmark native-runtime performance of List.Add with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }
    // Purpose: Benchmark native-runtime performance of List.Clear with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }
    // Purpose: Benchmark native-runtime performance of List.Contains with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }
    // Purpose: Benchmark native-runtime performance of List.IndexOf with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }
    // Purpose: Benchmark native-runtime performance of List.Remove with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }
    // Purpose: Benchmark native-runtime performance of List.RemoveAt with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { new List<int>().RemoveAt(42); }
    // Purpose: Benchmark native-runtime performance of List.Sort with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }
    // Purpose: Benchmark native-runtime performance of List.ToArray with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }
    // Purpose: Benchmark native-runtime performance of Dictionary.Add with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }
    // Purpose: Benchmark native-runtime performance of Dictionary.get_Count with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }
    // Purpose: Benchmark native-runtime performance of Dictionary.TryGetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }
    // Purpose: Benchmark native-runtime performance of Dictionary.ContainsKey with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }
    // Purpose: Benchmark native-runtime performance of Dictionary.Remove with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }
    // Purpose: Benchmark native-runtime performance of HashSet.Add with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }
    // Purpose: Benchmark native-runtime performance of HashSet.Contains with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }
    // Purpose: Benchmark native-runtime performance of HashSet.Remove with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }}
