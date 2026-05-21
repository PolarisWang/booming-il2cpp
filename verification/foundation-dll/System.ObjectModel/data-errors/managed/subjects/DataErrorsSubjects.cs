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
        try { _ = default(DataErrorsChangedEventArgs)!.PropertyName; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::.ctor:System.Void(System.String)
    public static void Subject_1()
    {
        new DataErrorsChangedEventArgs("hello");
    }

    // [2] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Subject_2()
    {
        // TODO: System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) could not be auto-generated
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean()
    public static void Subject_3()
    {
        try { _ = default(INotifyDataErrorInfo)!.HasErrors; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String)
    public static void Subject_4()
    {
        try { default(INotifyDataErrorInfo)!.GetErrors("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Subject_5()
    {
        // TODO: System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) could not be auto-generated
    }

}