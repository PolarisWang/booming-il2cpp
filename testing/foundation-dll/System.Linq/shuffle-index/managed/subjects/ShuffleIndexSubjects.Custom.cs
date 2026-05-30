using Chaos.TestFramework;
// Handwritten custom entry implementations for System.Linq shuffle & index operations.
// Covers ElementAt (Int32/Index), ElementAtOrDefault (Int32/Index),
// First/FirstOrDefault, Last/LastOrDefault, Index, Shuffle,
// Single/SingleOrDefault (various overloads with/without predicate and default value).
// Scalar results are called and discarded; IEnumerable<T> results are forced
// via IEnumerator<T> + MoveNext().

public static partial class ShuffleIndexSubjects
{
    // [0] Enumerable.ElementAt<TSource>(IEnumerable<TSource>, Int32)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.ElementAt(source, 2);
        }
        catch { }

    }

    // [1] Enumerable.ElementAt<TSource>(IEnumerable<TSource>, System.Index)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.ElementAt(source, new System.Index(2));
        }
        catch { }

    }

    // [2] Enumerable.ElementAtOrDefault<TSource>(IEnumerable<TSource>, Int32)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        int[] source = new int[] { 10, 20 };
        System.Linq.Enumerable.ElementAtOrDefault(source, 5);
        }
        catch { }

    }

    // [3] Enumerable.ElementAtOrDefault<TSource>(IEnumerable<TSource>, System.Index)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        int[] source = new int[] { 10, 20 };
        System.Linq.Enumerable.ElementAtOrDefault(source, new System.Index(5));
        }
        catch { }

    }

    // [4] Enumerable.First<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.First(source);
        }
        catch { }

    }

    // [5] Enumerable.First<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.First(source, (int x) => x > 3);
        }
        catch { }

    }

    // [6] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.FirstOrDefault(source);
        }
        catch { }

    }

    // [7] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        int[] source = new int[] { };
        System.Linq.Enumerable.FirstOrDefault(source, 42);
        }
        catch { }

    }

    // [8] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.FirstOrDefault(source, (int x) => x > 10);
        }
        catch { }

    }

    // [9] Enumerable.FirstOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.FirstOrDefault(source, (int x) => x > 10, 99);
        }
        catch { }

    }

    // [10] Enumerable.Index<TSource>(IEnumerable<TSource>) — .NET 9 API, replaced with Select
    [Fact]
    public static void CustomEntrySubject_10()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30 };
        System.Collections.Generic.IEnumerable<System.ValueTuple<int, int>> result = System.Linq.Enumerable.Select(source, (int value, int index) => new System.ValueTuple<int, int>(index, value));
        System.Collections.Generic.IEnumerator<System.ValueTuple<int, int>> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [11] Enumerable.Last<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_11()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.Last(source);
        }
        catch { }

    }

    // [12] Enumerable.Last<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_12()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.Last(source, (int x) => x < 4);
        }
        catch { }

    }

    // [13] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_13()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30 };
        System.Linq.Enumerable.LastOrDefault(source);
        }
        catch { }

    }

    // [14] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_14()
    {
        try
        {
        int[] source = new int[] { };
        System.Linq.Enumerable.LastOrDefault(source, 42);
        }
        catch { }

    }

    // [15] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_15()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.LastOrDefault(source, (int x) => x > 10);
        }
        catch { }

    }

    // [16] Enumerable.LastOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    [Fact]
    public static void CustomEntrySubject_16()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.Enumerable.LastOrDefault(source, (int x) => x > 10, 99);
        }
        catch { }

    }

    // [17] Enumerable.Shuffle<TSource>(IEnumerable<TSource>) — .NET 9 API, replaced with OrderBy
    [Fact]
    public static void CustomEntrySubject_17()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.OrderBy(source, (int x) => System.Random.Shared.Next());
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { }

    }

    // [18] Enumerable.Single<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_18()
    {
        try
        {
        int[] source = new int[] { 42 };
        System.Linq.Enumerable.Single(source);
        }
        catch { }

    }

    // [19] Enumerable.Single<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_19()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.Single(source, (int x) => x == 30);
        }
        catch { }

    }

    // [20] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>)
    [Fact]
    public static void CustomEntrySubject_20()
    {
        try
        {
        int[] source = new int[] { 42 };
        System.Linq.Enumerable.SingleOrDefault(source);
        }
        catch { }

    }

    // [21] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, TSource)
    [Fact]
    public static void CustomEntrySubject_21()
    {
        try
        {
        int[] source = new int[] { };
        System.Linq.Enumerable.SingleOrDefault(source, 99);
        }
        catch { }

    }

    // [22] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>)
    [Fact]
    public static void CustomEntrySubject_22()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.SingleOrDefault(source, (int x) => x == 100);
        }
        catch { }

    }

    // [23] Enumerable.SingleOrDefault<TSource>(IEnumerable<TSource>, Func<TSource,bool>, TSource)
    [Fact]
    public static void CustomEntrySubject_23()
    {
        try
        {
        int[] source = new int[] { 10, 20, 30, 40, 50 };
        System.Linq.Enumerable.SingleOrDefault(source, (int x) => x == 100, 99);
        }
        catch { }

    }
}
