using Chaos.TestFramework;
// Handwritten custom entry points
// Family: family/System.Linq/set-operations
// Assembly: System.Linq
// Variant: handwritten-custom

public static partial class SetOperationsSubjects
{
    // [0] Contains(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        System.Linq.Enumerable.Contains(new int[] { 1, 2, 3, 4, 5 }, 3);
        }
        catch { }

    }

    // [1] Contains(IEnumerable<int>, int, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        System.Linq.Enumerable.Contains(new int[] { 1, 2, 3, 4, 5 }, 3, System.Collections.Generic.EqualityComparer<int>.Default);
        }
        catch { }

    }

    // [2] Distinct(IEnumerable<int>)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        System.Linq.Enumerable.Distinct(new int[] { 1, 2, 2, 3, 3, 3 }).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [3] Distinct(IEnumerable<int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        System.Linq.Enumerable.Distinct(new int[] { 1, 2, 2, 3, 3, 3 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [4] Except(IEnumerable<int>, IEnumerable<int>)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.Linq.Enumerable.Except(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [5] Except(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.Linq.Enumerable.Except(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [6] ExceptBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        System.Linq.Enumerable.ExceptBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [7] ExceptBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        System.Linq.Enumerable.ExceptBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [8] Intersect(IEnumerable<int>, IEnumerable<int>)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        System.Linq.Enumerable.Intersect(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [9] Intersect(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        System.Linq.Enumerable.Intersect(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [10] IntersectBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    [Fact]
    public static void CustomEntrySubject_10()
    {
        try
        {
        System.Linq.Enumerable.IntersectBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [11] IntersectBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_11()
    {
        try
        {
        System.Linq.Enumerable.IntersectBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [12] SequenceEqual(IEnumerable<int>, IEnumerable<int>)
    [Fact]
    public static void CustomEntrySubject_12()
    {
        try
        {
        System.Linq.Enumerable.SequenceEqual(new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 });
        }
        catch { }

    }

    // [13] SequenceEqual(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_13()
    {
        try
        {
        System.Linq.Enumerable.SequenceEqual(new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 }, System.Collections.Generic.EqualityComparer<int>.Default);
        }
        catch { }

    }

    // [14] Union(IEnumerable<int>, IEnumerable<int>)
    [Fact]
    public static void CustomEntrySubject_14()
    {
        try
        {
        System.Linq.Enumerable.Union(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [15] Union(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_15()
    {
        try
        {
        System.Linq.Enumerable.Union(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [16] UnionBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    [Fact]
    public static void CustomEntrySubject_16()
    {
        try
        {
        System.Linq.Enumerable.UnionBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [17] UnionBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    [Fact]
    public static void CustomEntrySubject_17()
    {
        try
        {
        System.Linq.Enumerable.UnionBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }
}
