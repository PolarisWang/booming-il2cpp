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

// Auto-generated patch-side skeletons for FamilySystem.objectmodelDataErrorsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.objectmodelDataErrorsPatch
{
    // Purpose: Verify DataErrorsChangedEventArgs.DataErrorsChangedEventArgs: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::DataErrorsChangedEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_DataErrorsChangedEventArgs_Void_System_Object() { _ = DataErrorsChangedEventArgs.DataErrorsChangedEventArgs:(42); }
    // Purpose: Verify DataErrorsChangedEventArgs.get_PropertyName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_get_PropertyName_System_Void() { DataErrorsChangedEventArgs.PropertyName; }
    // Purpose: Verify DataErrorsChangedEventArgs.set_PropertyName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::set_PropertyName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_set_PropertyName_System_Void_P0() { DataErrorsChangedEventArgs.PropertyName = null!; }
    // Purpose: Verify INotifyDataErrorInfo.GetErrors executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_GetErrors_System_Void_P0() { INotifyDataErrorInfo.GetErrors(null!); }
    // Purpose: Verify INotifyDataErrorInfo.add_ErrorsChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_add_ErrorsChanged_System_Void_P0() { INotifyDataErrorInfo.add_ErrorsChanged(null!); }
    // Purpose: Verify INotifyDataErrorInfo.get_HasErrors executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_get_HasErrors_System_Void() { INotifyDataErrorInfo.HasErrors; }
    // Purpose: Verify INotifyDataErrorInfo.remove_ErrorsChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_remove_ErrorsChanged_System_Void_P0() { INotifyDataErrorInfo.remove_ErrorsChanged(null!); }
    // Purpose: Verify INotifyDataErrorInfo.set_HasErrors executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::set_HasErrors:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_set_HasErrors_System_Void_P0() { INotifyDataErrorInfo.HasErrors = null!; }}
