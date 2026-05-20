// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/aggregation
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class AggregationPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::Aggregate{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TSource,TSource})
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate}:TAccumulate(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate})
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Func{TAccumulate,TResult})
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,TAccumulate},System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource})
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Linq/System.Linq.Enumerable::CountBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,System.Int32}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource})
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Int32&)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}