// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/observable-collections
// Assembly: System.ObjectModel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;

public static partial class ObservableCollectionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void Method0()
    {
        try { ObservableCollection.add_CollectionChanged(null!); }
        catch { _exitCode = 1; }
    }

    // [1] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(System.Int32,System.Int32)
    public static void Method1()
    {
        try { ObservableCollection.Move(42, 42); }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static void Method2()
    {
        try { ObservableCollection.remove_CollectionChanged(null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void()
    public static void Method3()
    {
        try { new ObservableCollection(); }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.IEnumerable{T})
    public static void Method4()
    {
        try { new ObservableCollection(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.List{T})
    public static void Method5()
    {
        try { new ObservableCollection(null!); }
        catch { _exitCode = 1; }
    }

    // [6] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Collections.ObjectModel.ReadOnlyObservableCollection{T}()
    public static void Method6()
    {
        try { if ((int)(ReadOnlyObservableCollection.Empty) != (int)(ReadOnlyObservableCollection.Empty)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::.ctor:System.Void(System.Collections.ObjectModel.ObservableCollection{T})
    public static void Method7()
    {
        try { new ReadOnlyObservableCollection(null!); }
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
                case 6: Method6(); break;
                case 7: Method7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}