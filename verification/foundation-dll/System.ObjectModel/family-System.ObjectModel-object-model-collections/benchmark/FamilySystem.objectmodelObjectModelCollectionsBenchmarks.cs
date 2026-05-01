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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelObjectModelCollectionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelObjectModelCollectionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of KeyedCollection.KeyedCollection`2: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void() { _ = KeyedCollection.KeyedCollection`2:(); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.KeyedCollection`2: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void_System_Object() { _ = KeyedCollection.KeyedCollection`2:(42); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.KeyedCollection`2: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void_System_Object_System_Object() { _ = KeyedCollection.KeyedCollection`2:(42, 42); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.ChangeItemKey with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::ChangeItemKey:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_ChangeItemKey_System_Void_P0_P1() { KeyedCollection.ChangeItemKey(null!, null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.ClearItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::ClearItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_ClearItems_System_Void() { KeyedCollection.ClearItems(); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.Contains with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_Contains_System_Void_P0() { KeyedCollection.Contains(null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.GetKeyForItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::GetKeyForItem:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_GetKeyForItem_System_Void_P0() { KeyedCollection.GetKeyForItem(null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.InsertItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::InsertItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_InsertItem_System_Void_P0_P1() { KeyedCollection.InsertItem(null!, null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.Remove with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_Remove_System_Void_P0() { KeyedCollection.Remove(null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.RemoveItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::RemoveItem:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_RemoveItem_System_Void_P0() { KeyedCollection.RemoveItem(null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.SetItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::SetItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_SetItem_System_Void_P0_P1() { KeyedCollection.SetItem(null!, null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.TryGetValue with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_TryGetValue_System_Void_P0_P1() { KeyedCollection.TryGetValue(null!, null!); }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.get_Comparer with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Comparer_System_Void() { KeyedCollection.Comparer; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.get_Dictionary with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Dictionary:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Dictionary_System_Void() { KeyedCollection.Dictionary; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.get_Item with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Item_System_Void_P0() { KeyedCollection[null!]; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.set_Comparer with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Comparer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Comparer_System_Void_P0() { KeyedCollection.Comparer = null!; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.set_Dictionary with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Dictionary:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Dictionary_System_Void_P0() { KeyedCollection.Dictionary = null!; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.get_Item with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Item_System_Void() { KeyedCollection[]; }
    // Purpose: Benchmark native-runtime performance of KeyedCollection.set_Item with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Item:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Item_System_Void_P0() { KeyedCollection[null!]; }}
