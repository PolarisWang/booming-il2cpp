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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelCollectionChangeNotificationsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelCollectionChangeNotificationsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of INotifyCollectionChanged.add_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_add_CollectionChanged_System_Void_P0() { INotifyCollectionChanged.add_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyCollectionChanged.remove_CollectionChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_remove_CollectionChanged_System_Void_P0() { INotifyCollectionChanged.remove_CollectionChanged(null!); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42, 42); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42, 42, 42); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.get_Action with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_Action:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_Action_System_Void() { NotifyCollectionChangedEventArgs.Action; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.get_NewItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewItems_System_Void() { NotifyCollectionChangedEventArgs.NewItems; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.get_NewStartingIndex with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewStartingIndex:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewStartingIndex_System_Void() { NotifyCollectionChangedEventArgs.NewStartingIndex; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.get_OldItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldItems_System_Void() { NotifyCollectionChangedEventArgs.OldItems; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.get_OldStartingIndex with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldStartingIndex:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldStartingIndex_System_Void() { NotifyCollectionChangedEventArgs.OldStartingIndex; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.set_Action with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_Action:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_Action_System_Void_P0() { NotifyCollectionChangedEventArgs.Action = null!; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.set_NewItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewItems:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewItems_System_Void_P0() { NotifyCollectionChangedEventArgs.NewItems = null!; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.set_NewStartingIndex with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewStartingIndex:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewStartingIndex_System_Void_P0() { NotifyCollectionChangedEventArgs.NewStartingIndex = null!; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.set_OldItems with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldItems:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldItems_System_Void_P0() { NotifyCollectionChangedEventArgs.OldItems = null!; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventArgs.set_OldStartingIndex with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldStartingIndex:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldStartingIndex_System_Void_P0() { NotifyCollectionChangedEventArgs.OldStartingIndex = null!; }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventHandler.NotifyCollectionChangedEventHandler: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::NotifyCollectionChangedEventHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_NotifyCollectionChangedEventHandle() { _ = NotifyCollectionChangedEventHandler.NotifyCollectionChangedEventHandler:(42, 42); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventHandler.BeginInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_BeginInvoke_System_Void_P0_P1_P2_P() { NotifyCollectionChangedEventHandler.BeginInvoke(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventHandler.EndInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::EndInvoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_EndInvoke_System_Void_P0() { NotifyCollectionChangedEventHandler.EndInvoke(null!); }
    // Purpose: Benchmark native-runtime performance of NotifyCollectionChangedEventHandler.Invoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::Invoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_Invoke_System_Void_P0_P1() { NotifyCollectionChangedEventHandler.Invoke(null!, null!); }}
