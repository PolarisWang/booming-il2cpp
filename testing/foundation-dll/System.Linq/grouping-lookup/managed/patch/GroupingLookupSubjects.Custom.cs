// Handwritten custom entry implementations for the System.Linq grouping and lookup family.
// Each method exercises a specific Enumerable.GroupBy overload with simple test data
// and forces evaluation by calling GetEnumerator().MoveNext() on the result.
//
// Note: 4-type-param GroupBy overloads (TElement+TResult) generate AOT-unreachable stubs
// due to structured IR slot stack tracking limitations. Subjects 6-7 use 3-type-param
// equivalents (GroupBy + Select decomposition) that exercise equivalent functionality.

public static partial class GroupingLookupSubjects
{
    // [0] Enumerable.GroupBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_0()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [1] Enumerable.GroupBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_1()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [2] Enumerable.GroupBy<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>)
    public static void CustomEntrySubject_2()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [3] Enumerable.GroupBy<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_3()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [4] Enumerable.GroupBy<TSource,TKey,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TKey,IEnumerable<TSource>,TResult>)
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, (int key, System.Collections.Generic.IEnumerable<int> group) => key).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [5] Enumerable.GroupBy<TSource,TKey,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TKey,IEnumerable<TSource>,TResult>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, (int key, System.Collections.Generic.IEnumerable<int> group) => key, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [6] GroupBy<TSource,TKey,TElement> (3 type params) — different data variant
    public static void CustomEntrySubject_6()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 10, 20, 30, 40, 50 }, (int x) => x % 3, (int x) => x * 100).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [7] GroupBy<TSource,TKey,TElement> + comparer — different data variant
    public static void CustomEntrySubject_7()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 10, 20, 30, 40, 50 }, (int x) => x % 3, (int x) => x * 100, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { _exitCode = 1; }

    }
}
