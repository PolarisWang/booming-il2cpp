// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class PropertyChangeNotificationsSubjects
{
    // [0] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try {
            default(INotifyPropertyChanged)!.add_PropertyChanged(default);
        }
        catch { }
    }

    // [1] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try {
            default(INotifyPropertyChanged)!.remove_PropertyChanged(default);
        }
        catch { }
    }

    // [2] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try {
            default(INotifyPropertyChanging)!.add_PropertyChanging(default);
        }
        catch { }
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try {
            default(INotifyPropertyChanging)!.remove_PropertyChanging(default);
        }
        catch { }
    }

}
