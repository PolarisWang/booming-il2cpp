// Handwritten custom entry implementations for System.Linq ordering operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T>.

public static partial class OrderingSubjects
{
    // [0] Enumerable.Order<T>(IEnumerable<T>)
    public static void CustomEntrySubject_0()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.Order(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [1] Enumerable.Order<T>(IEnumerable<T>, IComparer<T>)
    public static void CustomEntrySubject_1()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.Order(source, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [2] Enumerable.OrderBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_2()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [3] Enumerable.OrderBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    public static void CustomEntrySubject_3()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [4] Enumerable.OrderByDescending<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_4()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderByDescending(source, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [5] Enumerable.OrderByDescending<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    public static void CustomEntrySubject_5()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderByDescending(source, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [6] Enumerable.OrderDescending<T>(IEnumerable<T>)
    public static void CustomEntrySubject_6()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderDescending(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [7] Enumerable.OrderDescending<T>(IEnumerable<T>, IComparer<T>)
    public static void CustomEntrySubject_7()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.OrderDescending(source, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [8] Enumerable.Reverse<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_8()
    {
        System.Collections.Generic.IEnumerable<int> source = System.Linq.Enumerable.Range(1, 3);
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Reverse(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [9] Enumerable.Reverse<TSource>(TSource[])
    public static void CustomEntrySubject_9()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Reverse(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [10] Enumerable.ThenBy<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_10()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenBy(ordered, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [11] Enumerable.ThenBy<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    public static void CustomEntrySubject_11()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenBy(ordered, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [12] Enumerable.ThenByDescending<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_12()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenByDescending(ordered, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [13] Enumerable.ThenByDescending<TSource,TKey>(IOrderedEnumerable<TSource>, Func<TSource,TKey>, IComparer<TKey>)
    public static void CustomEntrySubject_13()
    {
        int[] source = new int[] { 3, 1, 2 };
        System.Linq.IOrderedEnumerable<int> ordered = System.Linq.Enumerable.OrderBy(source, (int x) => x);
        System.Linq.IOrderedEnumerable<int> result = System.Linq.Enumerable.ThenByDescending(ordered, (int x) => x, System.Collections.Generic.Comparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }
}
