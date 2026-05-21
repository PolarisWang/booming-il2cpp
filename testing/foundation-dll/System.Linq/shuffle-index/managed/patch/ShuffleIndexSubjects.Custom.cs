// Handwritten custom entry implementations for System.Linq shuffle & index operations.
// Covers ElementAt (Int32/Index), ElementAtOrDefault (Int32/Index),
// First/FirstOrDefault, Last/LastOrDefault, Index, Shuffle,
// Single/SingleOrDefault (various overloads with/without predicate and default value).
// Scalar results are called and discarded; IEnumerable<T> results are forced
// via IEnumerator<T> + MoveNext().

public static partial class ShuffleIndexSubjects
{
    // [0] Enumerable.ElementAt<TSource>(IEnumerable<TSource>, Int32)
    public static void CustomEntrySubject_0()
    {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.ElementAt(source, 2);
    }

    // [1] Enumerable.ElementAt<TSource>(IEnumerable<TSource>, System.Index)
    public static void CustomEntrySubject_1()
    {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.ElementAt(source, new System.Index(2));
    }

    // [2] Enumerable.ElementAtOrDefault<TSource>(IEnumerable<TSource>, Int32)
    public static void CustomEntrySubject_2()
    {
        int[] source = new int[] { 10, 20 };
        System.Linq.Enumerable.ElementAtOrDefault(source, 5);
    }

    // [3] Enumerable.ElementAtOrDefault<TSource>(IEnumerable<TSource>, System.Index)
    public static void CustomEntrySubject_3()
    {
        int[] source = new int[] { 10, 20 };
        System.Linq.Enumerable.ElementAtOrDefault(source, new System.Index(5));
    }

    // [4] Enumerable.First<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_4()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.First(source);
    }

    // [5] Enumerable.First<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_5()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.First(source, (int x) => x > 3);
    }

    // [6] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_6()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.FirstOrDefault(source);
    }

    // [7] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_7()
    {
        int[] source = new int[] { };
        System.Linq.Enumerable.FirstOrDefault(source, 42);
    }

    // [8] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_8()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.FirstOrDefault(source, (int x) => x > 10);
    }

    // [9] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    public static void CustomEntrySubject_9()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.FirstOrDefault(source, (int x) => x > 10, 99);
    }

    // [10] Enumerable.Index<TSource>(IEnumerable<TSource>) — .NET 9 API, replaced with Select
    public static void CustomEntrySubject_10()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Collections.Generic.IEnumerable<System.ValueTuple<int, int>> result = System.Linq.Enumerable.Select(source, (int value, int index) => new System.ValueTuple<int, int>(index, value));
        System.Collections.Generic.IEnumerator<System.ValueTuple<int, int>> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [11] Enumerable.Last<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_11()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.Last(source);
    }

    // [12] Enumerable.Last<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_12()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.Last(source, (int x) => x < 4);
    }

    // [13] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_13()
    {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.LastOrDefault(source);
    }

    // [14] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_14()
    {
        int[] source = new int[] { };
        System.Linq.Enumerable.LastOrDefault(source, 42);
    }

    // [15] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_15()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.LastOrDefault(source, (int x) => x > 10);
    }

    // [16] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    public static void CustomEntrySubject_16()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.LastOrDefault(source, (int x) => x > 10, 99);
    }

    // [17] Enumerable.Shuffle<TSource>(IEnumerable<TSource>) — .NET 9 API, replaced with OrderBy
    public static void CustomEntrySubject_17()
    {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => System.Random.Shared.Next());
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
    }

    // [18] Enumerable.Single<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_18()
    {
        int[] source = new int[] { 42 };
        System.Linq.Enumerable.Single(source);
    }

    // [19] Enumerable.Single<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_19()
    {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.Single(source, (int x) => x == 30);
    }

    // [20] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_20()
    {
        int[] source = new int[] { 42 };
        System.Linq.Enumerable.SingleOrDefault(source);
    }

    // [21] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, TSource)
    public static void CustomEntrySubject_21()
    {
        int[] source = new int[] { };
        System.Linq.Enumerable.SingleOrDefault(source, 99);
    }

    // [22] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    public static void CustomEntrySubject_22()
    {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.SingleOrDefault(source, (int x) => x == 100);
    }

    // [23] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    public static void CustomEntrySubject_23()
    {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.SingleOrDefault(source, (int x) => x == 100, 99);
    }
}
