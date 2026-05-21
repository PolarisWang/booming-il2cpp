// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/object-model-collections
// Assembly: System.ObjectModel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

public static partial class ObjectModelCollectionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Boolean(TKey)
    public static void Method0()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Boolean(TKey) could not be auto-generated
    }

    // [1] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Collections.Generic.IEqualityComparer{TKey}()
    public static void Method1()
    {
        try { if ((int)(KeyedCollection.Comparer) != (int)(KeyedCollection.Comparer)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem(TKey)
    public static void Method2()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem(TKey) could not be auto-generated
    }

    // [3] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Boolean(TKey)
    public static void Method3()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Boolean(TKey) could not be auto-generated
    }

    // [4] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Boolean(TKey,TItem&)
    public static void Method4()
    {
        // TODO: System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Boolean(TKey,TItem&) could not be auto-generated
    }

    // [5] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem()
    public static void Method5()
    {
        try { if ((int)(KeyedCollection[]) != (int)(KeyedCollection[])) _exitCode = 1; }
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