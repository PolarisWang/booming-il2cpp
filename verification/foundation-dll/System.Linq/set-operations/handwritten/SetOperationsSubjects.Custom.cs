// Handwritten custom entry points
// Family: family/System.Linq/set-operations
// Assembly: System.Linq
// Variant: handwritten-custom

public static partial class SetOperationsSubjects
{
    // [0] Contains(IEnumerable<int>, int)
    public static void CustomEntrySubject_0()
    {
        System.Linq.Enumerable.Contains(new int[] { 1, 2, 3, 4, 5 }, 3);
    }

    // [1] Contains(IEnumerable<int>, int, IEqualityComparer<int>)
    public static void CustomEntrySubject_1()
    {
        System.Linq.Enumerable.Contains(new int[] { 1, 2, 3, 4, 5 }, 3, System.Collections.Generic.EqualityComparer<int>.Default);
    }

    // [2] Distinct(IEnumerable<int>)
    public static void CustomEntrySubject_2()
    {
        System.Linq.Enumerable.Distinct(new int[] { 1, 2, 2, 3, 3, 3 }).GetEnumerator().MoveNext();
    }

    // [3] Distinct(IEnumerable<int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_3()
    {
        System.Linq.Enumerable.Distinct(new int[] { 1, 2, 2, 3, 3, 3 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [4] Except(IEnumerable<int>, IEnumerable<int>)
    public static void CustomEntrySubject_4()
    {
        System.Linq.Enumerable.Except(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
    }

    // [5] Except(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_5()
    {
        System.Linq.Enumerable.Except(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [6] ExceptBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    public static void CustomEntrySubject_6()
    {
        System.Linq.Enumerable.ExceptBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
    }

    // [7] ExceptBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_7()
    {
        System.Linq.Enumerable.ExceptBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [8] Intersect(IEnumerable<int>, IEnumerable<int>)
    public static void CustomEntrySubject_8()
    {
        System.Linq.Enumerable.Intersect(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
    }

    // [9] Intersect(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_9()
    {
        System.Linq.Enumerable.Intersect(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [10] IntersectBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    public static void CustomEntrySubject_10()
    {
        System.Linq.Enumerable.IntersectBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
    }

    // [11] IntersectBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_11()
    {
        System.Linq.Enumerable.IntersectBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [12] SequenceEqual(IEnumerable<int>, IEnumerable<int>)
    public static void CustomEntrySubject_12()
    {
        System.Linq.Enumerable.SequenceEqual(new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 });
    }

    // [13] SequenceEqual(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_13()
    {
        System.Linq.Enumerable.SequenceEqual(new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 }, System.Collections.Generic.EqualityComparer<int>.Default);
    }

    // [14] Union(IEnumerable<int>, IEnumerable<int>)
    public static void CustomEntrySubject_14()
    {
        System.Linq.Enumerable.Union(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }).GetEnumerator().MoveNext();
    }

    // [15] Union(IEnumerable<int>, IEnumerable<int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_15()
    {
        System.Linq.Enumerable.Union(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [16] UnionBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>)
    public static void CustomEntrySubject_16()
    {
        System.Linq.Enumerable.UnionBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x).GetEnumerator().MoveNext();
    }

    // [17] UnionBy(IEnumerable<int>, IEnumerable<int>, Func<int,int>, IEqualityComparer<int>)
    public static void CustomEntrySubject_17()
    {
        System.Linq.Enumerable.UnionBy(new int[] { 1, 2, 3, 4, 5 }, new int[] { 4, 5, 6, 7, 8 }, (int x) => x, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }
}
