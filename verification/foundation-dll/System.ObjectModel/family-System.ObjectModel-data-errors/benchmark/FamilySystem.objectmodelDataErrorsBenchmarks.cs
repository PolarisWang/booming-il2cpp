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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelDataErrorsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelDataErrorsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of DataErrorsChangedEventArgs.DataErrorsChangedEventArgs: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::DataErrorsChangedEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_DataErrorsChangedEventArgs_Void_System_Object() { _ = DataErrorsChangedEventArgs.DataErrorsChangedEventArgs:(42); }
    // Purpose: Benchmark native-runtime performance of DataErrorsChangedEventArgs.get_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_get_PropertyName_System_Void() { DataErrorsChangedEventArgs.PropertyName; }
    // Purpose: Benchmark native-runtime performance of DataErrorsChangedEventArgs.set_PropertyName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::set_PropertyName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_set_PropertyName_System_Void_P0() { DataErrorsChangedEventArgs.PropertyName = null!; }
    // Purpose: Benchmark native-runtime performance of INotifyDataErrorInfo.GetErrors with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_GetErrors_System_Void_P0() { INotifyDataErrorInfo.GetErrors(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyDataErrorInfo.add_ErrorsChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_add_ErrorsChanged_System_Void_P0() { INotifyDataErrorInfo.add_ErrorsChanged(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyDataErrorInfo.get_HasErrors with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_get_HasErrors_System_Void() { INotifyDataErrorInfo.HasErrors; }
    // Purpose: Benchmark native-runtime performance of INotifyDataErrorInfo.remove_ErrorsChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_remove_ErrorsChanged_System_Void_P0() { INotifyDataErrorInfo.remove_ErrorsChanged(null!); }
    // Purpose: Benchmark native-runtime performance of INotifyDataErrorInfo.set_HasErrors with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::set_HasErrors:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_set_HasErrors_System_Void_P0() { INotifyDataErrorInfo.HasErrors = null!; }}
