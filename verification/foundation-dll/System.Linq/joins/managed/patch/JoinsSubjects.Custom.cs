// Handwritten custom entry implementations for System.Linq join operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by calling GetEnumerator().MoveNext() on the result.
// Covers Join, GroupJoin, LeftJoin, and RightJoin with and without IEqualityComparer.

public static partial class JoinsSubjects
{
    // [0] Enumerable.GroupJoin<TOuter,TInner,TKey,TResult>
    public static void CustomEntrySubject_0()
    {
        System.Linq.Enumerable.GroupJoin(
            new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 },
            (int x) => x, (int x) => x,
            (int o, System.Collections.Generic.IEnumerable<int> g) => o).GetEnumerator().MoveNext();
    }

    // [1] Enumerable.GroupJoin with comparer
    public static void CustomEntrySubject_1()
    {
        System.Linq.Enumerable.GroupJoin(
            new int[] { 1, 2, 3 }, new int[] { 1, 2, 3 },
            (int x) => x, (int x) => x,
            (int o, System.Collections.Generic.IEnumerable<int> g) => o,
            System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [2] Enumerable.Join<TOuter,TInner,TKey,TResult>
    public static void CustomEntrySubject_2()
    {
        System.Linq.Enumerable.Join(
            new int[] { 1, 2, 3 }, new int[] { 2, 3, 4 },
            (int x) => x, (int x) => x,
            (int o, int i) => o + i).GetEnumerator().MoveNext();
    }

    // [3] Enumerable.Join with comparer
    public static void CustomEntrySubject_3()
    {
        System.Linq.Enumerable.Join(
            new int[] { 1, 2, 3 }, new int[] { 2, 3, 4 },
            (int x) => x, (int x) => x,
            (int o, int i) => o + i,
            System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
    }

    // [4] LeftJoin — .NET 9 API, replaced with SelectMany + Where + DefaultIfEmpty
    public static void CustomEntrySubject_4()
    {
        System.Linq.Enumerable.SelectMany(
            new int[] { 1, 2, 3 },
            (int o) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 2, 3, 4 }, (int i) => i == o),
                    0),
                (int i) => o + i)).GetEnumerator().MoveNext();
    }

    // [5] LeftJoin with comparer — .NET 9 API replacement
    public static void CustomEntrySubject_5()
    {
        System.Linq.Enumerable.SelectMany(
            new int[] { 1, 2, 3 },
            (int o) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 2, 3, 4 }, (int i) => i == o),
                    0),
                (int i) => o + i)).GetEnumerator().MoveNext();
    }

    // [6] RightJoin — .NET 9 API, replaced with SelectMany on inner
    public static void CustomEntrySubject_6()
    {
        System.Linq.Enumerable.SelectMany(
            new int[] { 2, 3, 4 },
            (int i) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 1, 2, 3 }, (int o) => o == i),
                    0),
                (int o) => o + i)).GetEnumerator().MoveNext();
    }

    // [7] RightJoin with comparer — .NET 9 API replacement
    public static void CustomEntrySubject_7()
    {
        System.Linq.Enumerable.SelectMany(
            new int[] { 2, 3, 4 },
            (int i) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 1, 2, 3 }, (int o) => o == i),
                    0),
                (int o) => o + i)).GetEnumerator().MoveNext();
    }
}
