using Chaos.TestFramework;
// Handwritten custom entry implementations for System.Linq ordering operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T>.

public static partial class OrderingSubjects
{
    // [0] Enumerable.Order<T>(IEnumerable<T>)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.Order(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [1] Enumerable.Order<T>(IEnumerable<T>, IComparer<T>)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.Order(source, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [2] Enumerable.OrderBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [3] Enumerable.OrderBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [4] Enumerable.OrderByDescending<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderByDescending(source, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [5] Enumerable.OrderByDescending<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderByDescending(source, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [6] Enumerable.OrderDescending<T>(IEnumerable<T>)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderDescending(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [7] Enumerable.OrderDescending<T>(IEnumerable<T>, IComparer<T>)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderDescending(source, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [8] Enumerable.Reverse<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        System.Collections.Generic.IEnumerable<int> source = System.Linq.Enumerable.Range(1, 3);
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Reverse(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [9] Enumerable.Reverse<TSource>(TSource[])
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Reverse(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [10] Enumerable.ThenBy<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>)
    [Fact]
    public static void CustomEntrySubject_10()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenBy(ordered, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [11] Enumerable.ThenBy<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    [Fact]
    public static void CustomEntrySubject_11()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenBy(ordered, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [12] Enumerable.ThenByDescending<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>)
    [Fact]
    public static void CustomEntrySubject_12()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenByDescending(ordered, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [13] Enumerable.ThenByDescending<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    [Fact]
    public static void CustomEntrySubject_13()
    {
        try
        {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenByDescending(ordered, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }
}
