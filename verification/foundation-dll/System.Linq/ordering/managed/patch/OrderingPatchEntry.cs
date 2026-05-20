// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/ordering
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class OrderingPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T})
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T})
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T})
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T})
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource})
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(TSource{})
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey})
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey})
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
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
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}