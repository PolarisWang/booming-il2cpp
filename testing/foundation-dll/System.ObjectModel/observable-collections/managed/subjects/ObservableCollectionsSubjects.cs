// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/observable/collections
// Assembly: System.ObjectModel
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;

public static partial class ObservableCollectionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void Subject_0()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler) could not be auto-generated
    }

    // [1] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(System.Int32,System.Int32)
    public static void Subject_1()
    {
        try { default(ObservableCollection<byte>)!.Move(42, 42); }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void Subject_2()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler) could not be auto-generated
    }

    // [3] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void()
    public static void Subject_3()
    {
        try { new ObservableCollection<byte>(); }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.IEnumerable{T})
    public static void Subject_4()
    {
        try { new ObservableCollection<byte>(default); }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.List{T})
    public static void Subject_5()
    {
        try { new ObservableCollection<byte>(default); }
        catch { _exitCode = 1; }
    }

    // [6] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Collections.ObjectModel.ReadOnlyObservableCollection{T}()
    public static void Subject_6()
    {
        try { if (((ReadOnlyObservableCollection<byte>.Empty).GetHashCode()) != ((ReadOnlyObservableCollection<byte>.Empty).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::.ctor:System.Void(System.Collections.ObjectModel.ObservableCollection{T})
    public static void Subject_7()
    {
        try { new ReadOnlyObservableCollection<byte>(default); }
        catch { _exitCode = 1; }
    }

}