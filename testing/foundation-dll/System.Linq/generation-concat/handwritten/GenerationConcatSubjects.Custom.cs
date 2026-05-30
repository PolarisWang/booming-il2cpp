using Chaos.TestFramework;
// Handwritten custom entry implementations for System.Linq generation and concatenation operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T>.

public static partial class GenerationConcatSubjects
{
    // [0] Enumerable.Append<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Append(source, 4);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [1] Enumerable.Concat<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        int[] first = new int[] { 1, 2 };
        int[] second = new int[] { 3, 4 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Concat(first, second);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [2] Enumerable.DefaultIfEmpty<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DefaultIfEmpty(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [3] Enumerable.DefaultIfEmpty<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DefaultIfEmpty(source, 42);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [4] Enumerable.Empty<TResult>()
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Empty<int>();
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [5] Enumerable.InfiniteSequence<T>(T, T) — .NET 9 API, replaced with Range
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Range(0, 100);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [6] Enumerable.Prepend<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        int[] source = new int[] { 2, 3, 4 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Prepend(source, 1);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [7] Enumerable.Range(int, int)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Range(1, 5);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [8] Enumerable.Repeat<TResult>(TResult, int)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Repeat(42, 3);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [9] Enumerable.Sequence<T>(T, T, T) — .NET 9 API, replaced with array literal
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> result = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }
}
