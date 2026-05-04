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

// Auto-generated skeleton for Generic Collections Core (family/System.Private.CoreLib/collections/generic-core).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class CollectionsGenericCore
{
    // List.Add
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }

    // List.Clear
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void()
    {
    new List<int>().Clear();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }

    // List.Contains
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }

    // List.IndexOf
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }

    // List.Remove
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }

    // List.RemoveAt
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32()
    {
    // TODO: needs-manual — RemoveAt with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { }

    // List.Sort
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void()
    {
    new List<int>().Sort();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }

    // List.ToArray
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T()
    {
    _ = new List<int>().ToArray();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }

    // Dictionary.Add
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }

    // Dictionary.get_Count
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32()
    {
    var result = new Dictionary<string, int>().Count;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }

    // Dictionary.TryGetValue
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }

    // Dictionary.ContainsKey
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey()
    {
    // TODO: needs-manual — ContainsKey with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }

    // Dictionary.Remove
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }

    // HashSet.Add
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T()
    {
    // TODO: needs-manual — Add with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }

    // HashSet.Contains
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T()
    {
    // TODO: needs-manual — Contains with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }

    // HashSet.Remove
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    public static void Benchmark_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }
}
