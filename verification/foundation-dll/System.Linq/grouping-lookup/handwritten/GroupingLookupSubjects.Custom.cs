// Handwritten custom entry implementations for the System.Linq grouping and lookup family.
// Each method exercises a specific Enumerable.GroupBy overload with simple test data
// and forces evaluation by calling GetEnumerator().MoveNext() on the result.

public static partial class GroupingLookupSubjects
{
    // [0] Enumerable.GroupBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_0()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3).GetEnumerator().MoveNext();
    }

    // [1] Enumerable.GroupBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_1()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [2] Enumerable.GroupBy<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>)
    public static void CustomEntrySubject_2()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10).GetEnumerator().MoveNext();
    }

    // [3] Enumerable.GroupBy<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_3()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [4] Enumerable.GroupBy<TSource,TKey,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TKey,IEnumerable<TSource>,TResult>)
    public static void CustomEntrySubject_4()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, (int key, System.Collections.Generic.IEnumerable<int> group) => key).GetEnumerator().MoveNext();
    }

    // [5] Enumerable.GroupBy<TSource,TKey,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TKey,IEnumerable<TSource>,TResult>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_5()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, (int key, System.Collections.Generic.IEnumerable<int> group) => key, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [6] Enumerable.GroupBy<TSource,TKey,TElement,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, Func<TKey,IEnumerable<TElement>,TResult>)
    public static void CustomEntrySubject_6()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10, (int key, System.Collections.Generic.IEnumerable<int> group) => key).GetEnumerator().MoveNext();
    }

    // [7] Enumerable.GroupBy<TSource,TKey,TElement,TResult>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, Func<TKey,IEnumerable<TElement>,TResult>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_7()
    {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2, (int x) => x * 10, (int key, System.Collections.Generic.IEnumerable<int> group) => key, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }
}
