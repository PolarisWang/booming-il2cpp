// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/filtering
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class FilteringPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::All{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Linq/System.Linq.Enumerable::Any{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource})
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Linq/System.Linq.Enumerable::Any{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Linq/System.Linq.Enumerable::Cast{TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.IEnumerable)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Linq/System.Linq.Enumerable::OfType{TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.IEnumerable)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Linq/System.Linq.Enumerable::Where{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Linq/System.Linq.Enumerable::Where{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Boolean})
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
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
                case 8: Subject_8(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}