// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ObservableCollectionsSubjects
{
    // [0] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void CustomEntrySubject_0()
    {
        try {
            default(ObservableCollection<byte>)!.add_CollectionChanged(default);
        }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void CustomEntrySubject_2()
    {
        try {
            default(ObservableCollection<byte>)!.remove_CollectionChanged(default);
        }
        catch { _exitCode = 1; }
    }

}
