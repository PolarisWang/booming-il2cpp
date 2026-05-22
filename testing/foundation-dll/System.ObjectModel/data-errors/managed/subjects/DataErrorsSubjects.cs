// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/data/errors
// Assembly: System.ObjectModel
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

public static partial class DataErrorsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.String()
    public static void Subject_0()
    {
        try { if (((default(DataErrorsChangedEventArgs)!.PropertyName).Length) != ((default(DataErrorsChangedEventArgs)!.PropertyName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::.ctor:System.Void(System.String)
    public static void Subject_1()
    {
        try { new DataErrorsChangedEventArgs("hello"); }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Subject_2()
    {
        // TODO: System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) could not be auto-generated
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean()
    public static void Subject_3()
    {
        try { if (((default(INotifyDataErrorInfo)!.HasErrors) ? 1 : 0) != ((default(INotifyDataErrorInfo)!.HasErrors) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String)
    public static void Subject_4()
    {
        try { if (((default(INotifyDataErrorInfo)!.GetErrors("hello")).GetHashCode()) != ((default(INotifyDataErrorInfo)!.GetErrors("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Subject_5()
    {
        // TODO: System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) could not be auto-generated
    }

}