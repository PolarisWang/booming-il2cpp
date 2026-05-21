// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/model/collections
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ObjectModelCollectionsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Boolean(TKey)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Collections.Generic.IEqualityComparer{TKey}()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem(TKey)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Boolean(TKey)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Boolean(TKey,TItem&)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}