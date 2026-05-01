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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelPropertyChangeNotificationsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelPropertyChangeNotificationsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of INotifyPropertyChanged.add_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyPropertyChanged_add_PropertyChanged_System_Void_P0() { INotifyPropertyChanged.add_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyPropertyChanged.remove_PropertyChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyPropertyChanged_remove_PropertyChanged_System_Void_P0() { INotifyPropertyChanged.remove_PropertyChanged(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyPropertyChanging.add_PropertyChanging with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyPropertyChanging_add_PropertyChanging_System_Void_P0() { INotifyPropertyChanging.add_PropertyChanging(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyPropertyChanging.remove_PropertyChanging with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyPropertyChanging_remove_PropertyChanging_System_Void_P0() { INotifyPropertyChanging.remove_PropertyChanging(null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventArgs.PropertyChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::PropertyChangedEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_PropertyChangedEventArgs_Void_System_Object() { _ = PropertyChangedEventArgs.PropertyChangedEventArgs:(42); }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventArgs.get_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::get_PropertyName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_get_PropertyName_System_Void() { PropertyChangedEventArgs.PropertyName; }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventArgs.set_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::set_PropertyName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_set_PropertyName_System_Void_P0() { PropertyChangedEventArgs.PropertyName = null!; }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventHandler.PropertyChangedEventHandler: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::PropertyChangedEventHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_PropertyChangedEventHandler_Void_System_Object_Syst() { _ = PropertyChangedEventHandler.PropertyChangedEventHandler:(42, 42); }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventHandler.BeginInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_BeginInvoke_System_Void_P0_P1_P2_P3() { PropertyChangedEventHandler.BeginInvoke(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventHandler.EndInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::EndInvoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_EndInvoke_System_Void_P0() { PropertyChangedEventHandler.EndInvoke(null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangedEventHandler.Invoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::Invoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_Invoke_System_Void_P0_P1() { PropertyChangedEventHandler.Invoke(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventArgs.PropertyChangingEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::PropertyChangingEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_PropertyChangingEventArgs_Void_System_Object() { _ = PropertyChangingEventArgs.PropertyChangingEventArgs:(42); }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventArgs.get_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::get_PropertyName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_get_PropertyName_System_Void() { PropertyChangingEventArgs.PropertyName; }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventArgs.set_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::set_PropertyName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_set_PropertyName_System_Void_P0() { PropertyChangingEventArgs.PropertyName = null!; }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventHandler.PropertyChangingEventHandler: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::PropertyChangingEventHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_PropertyChangingEventHandler_Void_System_Object_Sy() { _ = PropertyChangingEventHandler.PropertyChangingEventHandler:(42, 42); }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventHandler.BeginInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_BeginInvoke_System_Void_P0_P1_P2_P3() { PropertyChangingEventHandler.BeginInvoke(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventHandler.EndInvoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::EndInvoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_EndInvoke_System_Void_P0() { PropertyChangingEventHandler.EndInvoke(null!); }
    // Purpose: Benchmark native-runtime performance of PropertyChangingEventHandler.Invoke with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::Invoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_Invoke_System_Void_P0_P1() { PropertyChangingEventHandler.Invoke(null!, null!); }}
