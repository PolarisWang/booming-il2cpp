// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/data-errors
// Assembly: System.ObjectModel
// Variant: benchmark

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

public static partial class DataErrorsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.String()
    public static void Method0()
    {
        try { if (((DataErrorsChangedEventArgs.PropertyName).Length) != ((DataErrorsChangedEventArgs.PropertyName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::.ctor:System.Void(System.String)
    public static void Method1()
    {
        try { new DataErrorsChangedEventArgs("hello"); }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Method2()
    {
        try { INotifyDataErrorInfo.add_ErrorsChanged(null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean()
    public static void Method3()
    {
        try { if (((INotifyDataErrorInfo.HasErrors) ? 1 : 0) != ((INotifyDataErrorInfo.HasErrors) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String)
    public static void Method4()
    {
        try { if ((int)(INotifyDataErrorInfo.GetErrors("hello")) != (int)(INotifyDataErrorInfo.GetErrors("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    public static void Method5()
    {
        try { INotifyDataErrorInfo.remove_ErrorsChanged(null!); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}