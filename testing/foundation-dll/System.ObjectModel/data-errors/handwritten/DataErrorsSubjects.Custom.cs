// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class DataErrorsSubjects
{
    // [2] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { default(INotifyDataErrorInfo)!.add_ErrorsChanged(default); } catch { }    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = default(INotifyDataErrorInfo)!.HasErrors; } catch { }    }

    // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String)
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { _ = default(INotifyDataErrorInfo)!.GetErrors("hello"); } catch { }    }

    // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { default(INotifyDataErrorInfo)!.remove_ErrorsChanged(default); } catch { }    }

}
