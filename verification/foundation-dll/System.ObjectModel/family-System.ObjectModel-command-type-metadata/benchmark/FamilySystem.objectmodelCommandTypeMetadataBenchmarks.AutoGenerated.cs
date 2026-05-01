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

// Auto-generated benchmark skeletons for FamilySystem.objectmodelCommandTypeMetadataBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.objectmodelCommandTypeMetadataBenchmarks
{
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.TypeConverterAttribute: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void() { _ = TypeConverterAttribute.TypeConverterAttribute:(); }
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.TypeConverterAttribute: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void_System_Object() { _ = TypeConverterAttribute.TypeConverterAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.Equals with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_Equals_System_Void_P0() { TypeConverterAttribute.Equals(null!); }
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.GetHashCode with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_GetHashCode_System_Void() { TypeConverterAttribute.GetHashCode(); }
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.get_ConverterTypeName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_get_ConverterTypeName_System_Void() { TypeConverterAttribute.ConverterTypeName; }
    // Purpose: Benchmark native-runtime performance of TypeConverterAttribute.set_ConverterTypeName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::set_ConverterTypeName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_set_ConverterTypeName_System_Void_P0() { TypeConverterAttribute.ConverterTypeName = null!; }
    // Purpose: Benchmark native-runtime performance of ICustomTypeProvider.GetCustomType with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Reflection_ICustomTypeProvider_GetCustomType_System_Void() { ICustomTypeProvider.GetCustomType(); }
    // Purpose: Benchmark native-runtime performance of ICommand.CanExecute with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Input_ICommand_CanExecute_System_Void_P0() { ICommand.CanExecute(null!); }
    // Purpose: Benchmark native-runtime performance of ICommand.Execute with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Input_ICommand_Execute_System_Void_P0() { ICommand.Execute(null!); }
    // Purpose: Benchmark native-runtime performance of ICommand.add_CanExecuteChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Input_ICommand_add_CanExecuteChanged_System_Void_P0() { ICommand.add_CanExecuteChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ICommand.remove_CanExecuteChanged with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Input_ICommand_remove_CanExecuteChanged_System_Void_P0() { ICommand.remove_CanExecuteChanged(null!); }
    // Purpose: Benchmark native-runtime performance of ValueSerializerAttribute.ValueSerializerAttribute: with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::ValueSerializerAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_ValueSerializerAttribute_Void_System_Object() { _ = ValueSerializerAttribute.ValueSerializerAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of ValueSerializerAttribute.get_ValueSerializerType with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerType_System_Void() { ValueSerializerAttribute.ValueSerializerType; }
    // Purpose: Benchmark native-runtime performance of ValueSerializerAttribute.get_ValueSerializerTypeName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerTypeName_System_Void() { ValueSerializerAttribute.ValueSerializerTypeName; }
    // Purpose: Benchmark native-runtime performance of ValueSerializerAttribute.set_ValueSerializerType with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerType_System_Void_P0() { ValueSerializerAttribute.ValueSerializerType = null!; }
    // Purpose: Benchmark native-runtime performance of ValueSerializerAttribute.set_ValueSerializerTypeName with typical input
    [BenchmarkSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerTypeName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerTypeName_System_Void_P0() { ValueSerializerAttribute.ValueSerializerTypeName = null!; }}
