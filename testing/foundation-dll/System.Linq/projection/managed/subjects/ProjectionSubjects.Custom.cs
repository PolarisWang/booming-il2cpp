// Handwritten custom entry implementations for System.Linq projection operations.
// Covers Select (with/without index), SelectMany (with/without index,
// with/without result selector), and Zip (2-sequence, 3-sequence, with/without result selector).
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T>.

public static partial class ProjectionSubjects
{
    // [0] Enumerable.Select<TSource,TResult>(IEnumerable<TSource>, Func<TSource,TResult>)
    public static void CustomEntrySubject_0()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Select(source, (int x) => x * 2);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [1] Enumerable.Select<TSource,TResult>(IEnumerable<TSource>, Func<TSource,int,TResult>)
    public static void CustomEntrySubject_1()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Select(source, (int x, int i) => x + i);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [2] Enumerable.SelectMany<TSource,TResult>(IEnumerable<TSource>, Func<TSource,IEnumerable<TResult>>)
    public static void CustomEntrySubject_2()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.SelectMany(source, (int x) => new int[] { x, x * 10 });
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [3] Enumerable.SelectMany<TSource,TResult>(IEnumerable<TSource>, Func<TSource,int,IEnumerable<TResult>>)
    public static void CustomEntrySubject_3()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.SelectMany(source, (int x, int i) => new int[] { x, i });
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [4] Enumerable.SelectMany<TSource,TCollection,TResult>(IEnumerable<TSource>, Func<TSource,int,IEnumerable<TCollection>>, Func<TSource,TCollection,TResult>)
    public static void CustomEntrySubject_4()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.SelectMany(source, (int x, int i) => new int[] { x, i }, (int x, int c) => x + c);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [5] Enumerable.SelectMany<TSource,TCollection,TResult>(IEnumerable<TSource>, Func<TSource,IEnumerable<TCollection>>, Func<TSource,TCollection,TResult>)
    public static void CustomEntrySubject_5()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.SelectMany(source, (int x) => new int[] { x, x * 10 }, (int x, int c) => x + c);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [6] Enumerable.Zip<TFirst,TSecond,TResult>(IEnumerable<TFirst>, IEnumerable<TSecond>, Func<TFirst,TSecond,TResult>)
    public static void CustomEntrySubject_6()
    {
        int[] first = new int[] { 1, 2, 3 };
        int[] second = new int[] { 10, 20, 30 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Zip(first, second, (int f, int s) => f + s);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [7] Enumerable.Zip<TFirst,TSecond>(IEnumerable<TFirst>, IEnumerable<TSecond>)
    public static void CustomEntrySubject_7()
    {
        int[] first = new int[] { 1, 2, 3 };
        int[] second = new int[] { 10, 20, 30 };
        System.Collections.Generic.IEnumerable<System.ValueTuple<int, int>> result = System.Linq.Enumerable.Zip(first, second);
        System.Collections.Generic.IEnumerator<System.ValueTuple<int, int>> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [8] Enumerable.Zip<TFirst,TSecond,TThird>(IEnumerable<TFirst>, IEnumerable<TSecond>, IEnumerable<TThird>)
    public static void CustomEntrySubject_8()
    {
        int[] first = new int[] { 1, 2, 3 };
        int[] second = new int[] { 10, 20, 30 };
        int[] third = new int[] { 100, 200, 300 };
        System.Collections.Generic.IEnumerable<System.ValueTuple<int, int, int>> result = System.Linq.Enumerable.Zip(first, second, third);
        System.Collections.Generic.IEnumerator<System.ValueTuple<int, int, int>> e = result.GetEnumerator();
        e.MoveNext();
    }
}
