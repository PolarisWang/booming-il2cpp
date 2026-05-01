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

// Auto-generated hot-update skeletons for FamilySystem.objectmodelCommandTypeMetadataHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.objectmodelCommandTypeMetadataHotUpdate
{
    // Purpose: Verify TypeConverterAttribute.TypeConverterAttribute: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void() { _ = TypeConverterAttribute.TypeConverterAttribute:(); }
    // Purpose: Verify TypeConverterAttribute.TypeConverterAttribute: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void_System_Object() { _ = TypeConverterAttribute.TypeConverterAttribute:(42); }
    // Purpose: Verify TypeConverterAttribute.Equals executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_Equals_System_Void_P0() { TypeConverterAttribute.Equals(null!); }
    // Purpose: Verify TypeConverterAttribute.GetHashCode executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_GetHashCode_System_Void() { TypeConverterAttribute.GetHashCode(); }
    // Purpose: Verify TypeConverterAttribute.get_ConverterTypeName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_get_ConverterTypeName_System_Void() { TypeConverterAttribute.ConverterTypeName; }
    // Purpose: Verify TypeConverterAttribute.set_ConverterTypeName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::set_ConverterTypeName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_ComponentModel_TypeConverterAttribute_set_ConverterTypeName_System_Void_P0() { TypeConverterAttribute.ConverterTypeName = null!; }
    // Purpose: Verify ICustomTypeProvider.GetCustomType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Reflection_ICustomTypeProvider_GetCustomType_System_Void() { ICustomTypeProvider.GetCustomType(); }
    // Purpose: Verify ICommand.CanExecute executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Input_ICommand_CanExecute_System_Void_P0() { ICommand.CanExecute(null!); }
    // Purpose: Verify ICommand.Execute executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Input_ICommand_Execute_System_Void_P0() { ICommand.Execute(null!); }
    // Purpose: Verify ICommand.add_CanExecuteChanged executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Input_ICommand_add_CanExecuteChanged_System_Void_P0() { ICommand.add_CanExecuteChanged(null!); }
    // Purpose: Verify ICommand.remove_CanExecuteChanged executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Input_ICommand_remove_CanExecuteChanged_System_Void_P0() { ICommand.remove_CanExecuteChanged(null!); }
    // Purpose: Verify ValueSerializerAttribute.ValueSerializerAttribute: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::ValueSerializerAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_ValueSerializerAttribute_Void_System_Object() { _ = ValueSerializerAttribute.ValueSerializerAttribute:(42); }
    // Purpose: Verify ValueSerializerAttribute.get_ValueSerializerType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerType_System_Void() { ValueSerializerAttribute.ValueSerializerType; }
    // Purpose: Verify ValueSerializerAttribute.get_ValueSerializerTypeName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerTypeName_System_Void() { ValueSerializerAttribute.ValueSerializerTypeName; }
    // Purpose: Verify ValueSerializerAttribute.set_ValueSerializerType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerType_System_Void_P0() { ValueSerializerAttribute.ValueSerializerType = null!; }
    // Purpose: Verify ValueSerializerAttribute.set_ValueSerializerTypeName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerTypeName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerTypeName_System_Void_P0() { ValueSerializerAttribute.ValueSerializerTypeName = null!; }}
