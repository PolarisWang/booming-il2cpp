// Handwritten custom entry implementations for System.Linq filtering operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T> or calling a terminal operation.

public static partial class FilteringSubjects
{
    // [0] Enumerable.All<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_0()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Linq.Enumerable.All(source, (int x) => x > 0);
    }

    // [1] Enumerable.Any<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_1()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Linq.Enumerable.Any(source);
    }

    // [2] Enumerable.Any<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_2()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Linq.Enumerable.Any(source, (int x) => x > 2);
    }

    // [3] Enumerable.Cast<TResult>(IEnumerable)
    public static void CustomEntrySubject_3()
    {
        System.Collections.ArrayList list = new System.Collections.ArrayList();
        list.Add(1);
        list.Add(2);
        list.Add(3);
        System.Collections.IEnumerable result = System.Linq.Enumerable.Cast<int>(list);
        System.Collections.IEnumerator e = result.GetEnumerator();
        e.MoveNext();
    }

    // [4] Enumerable.DistinctBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_4()
    {
        int[] source = new int[] { 1, 2, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DistinctBy(source, (int x) => x);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [5] Enumerable.DistinctBy<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_5()
    {
        int[] source = new int[] { 1, 2, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DistinctBy(source, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [6] Enumerable.OfType<TResult>(IEnumerable)
    public static void CustomEntrySubject_6()
    {
        System.Collections.ArrayList list = new System.Collections.ArrayList();
        list.Add(1);
        list.Add("hello");
        list.Add(3);
        System.Collections.IEnumerable result = System.Linq.Enumerable.OfType<int>(list);
        System.Collections.IEnumerator e = result.GetEnumerator();
        e.MoveNext();
    }

    // [7] Enumerable.Where<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_7()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Where(source, (int x) => x > 2);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [8] Enumerable.Where<TSource>(IEnumerable<TSource>, Func<TSource,int,bool>)
    public static void CustomEntrySubject_8()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Where(source, (int x, int i) => i > 1);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }
}
