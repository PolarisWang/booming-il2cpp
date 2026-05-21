// Handwritten custom entry implementations for System.Linq generation and concatenation operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by enumerating the result via IEnumerator<T>.

public static partial class GenerationConcatSubjects
{
    // [0] Enumerable.Append<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_0()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Append(source, 4);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [1] Enumerable.Concat<TSource>(IEnumerable<TSource>, IEnumerable<TSource>)
    public static void CustomEntrySubject_1()
    {
        int[] first = new int[] { 1, 2 };
        int[] second = new int[] { 3, 4 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Concat(first, second);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [2] Enumerable.DefaultIfEmpty<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_2()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DefaultIfEmpty(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [3] Enumerable.DefaultIfEmpty<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_3()
    {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.DefaultIfEmpty(source, 42);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [4] Enumerable.Empty<TResult>()
    public static void CustomEntrySubject_4()
    {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Empty<int>();
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [5] Enumerable.InfiniteSequence<T>(T, T) — .NET 9 API, replaced with Range
    public static void CustomEntrySubject_5()
    {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Range(0, 100);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [6] Enumerable.Prepend<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_6()
    {
        int[] source = new int[] { 2, 3, 4 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Prepend(source, 1);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [7] Enumerable.Range(int, int)
    public static void CustomEntrySubject_7()
    {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Range(1, 5);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [8] Enumerable.Repeat<TResult>(TResult, int)
    public static void CustomEntrySubject_8()
    {
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.Repeat(42, 3);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [9] Enumerable.Sequence<T>(T, T, T) — .NET 9 API, replaced with array literal
    public static void CustomEntrySubject_9()
    {
        System.Collections.Generic.IEnumerable<int> result = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }
}
