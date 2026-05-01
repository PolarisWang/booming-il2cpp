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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelObservableCollectionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelObservableCollectionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ObservableCollection.ObservableCollection`1: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void() { _ = ObservableCollection.ObservableCollection`1:(); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.ObservableCollection`1: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void_System_Object() { _ = ObservableCollection.ObservableCollection`1:(42); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.BlockReentrancy with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::BlockReentrancy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_BlockReentrancy_System_Void() { ObservableCollection.BlockReentrancy(); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.CheckReentrancy with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::CheckReentrancy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_CheckReentrancy_System_Void() { ObservableCollection.CheckReentrancy(); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.ClearItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ClearItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ClearItems_System_Void() { ObservableCollection.ClearItems(); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.InsertItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::InsertItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_InsertItem_System_Void_P0_P1() { ObservableCollection.InsertItem(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.Move with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_Move_System_Void_P0_P1() { ObservableCollection.Move(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.MoveItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::MoveItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_MoveItem_System_Void_P0_P1() { ObservableCollection.MoveItem(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.OnCollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnCollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnCollectionChanged_System_Void_P0() { ObservableCollection.OnCollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.OnPropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnPropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnPropertyChanged_System_Void_P0() { ObservableCollection.OnPropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.RemoveItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::RemoveItem:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_RemoveItem_System_Void_P0() { ObservableCollection.RemoveItem(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.SetItem with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::SetItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_SetItem_System_Void_P0_P1() { ObservableCollection.SetItem(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.add_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_CollectionChanged_System_Void_P0() { ObservableCollection.add_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.add_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_PropertyChanged_System_Void_P0() { ObservableCollection.add_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.remove_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_CollectionChanged_System_Void_P0() { ObservableCollection.remove_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ObservableCollection.remove_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_PropertyChanged_System_Void_P0() { ObservableCollection.remove_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.ReadOnlyObservableCollection`1: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::ReadOnlyObservableCollection`1::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_ReadOnlyObservableCollection_1_Void_Sys() { _ = ReadOnlyObservableCollection.ReadOnlyObservableCollection`1:(42); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.OnCollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnCollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnCollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.OnCollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.OnPropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnPropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnPropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.OnPropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.add_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_CollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.add_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.add_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_PropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.add_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.get_Empty with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_get_Empty_System_Void() { ReadOnlyObservableCollection.Empty; }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.remove_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_CollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.remove_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.remove_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_PropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.remove_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ReadOnlyObservableCollection.set_Empty with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::set_Empty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_set_Empty_System_Void_P0() { ReadOnlyObservableCollection.Empty = null!; }}
