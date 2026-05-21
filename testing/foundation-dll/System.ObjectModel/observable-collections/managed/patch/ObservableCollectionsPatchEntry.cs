// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/observable/collections
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ObservableCollectionsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(System.Int32,System.Int32)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.IEnumerable{T})
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.List{T})
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Collections.ObjectModel.ReadOnlyObservableCollection{T}()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::.ctor:System.Void(System.Collections.ObjectModel.ObservableCollection{T})
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}