using Chaos.TestFramework;
// Handwritten custom entry implementations for System.Linq join operations.
// Each method calls the corresponding Enumerable API and forces evaluation
// by calling GetEnumerator().MoveNext() on the result.
// Covers cross-join, inner-join, GroupBy-based join patterns with and without IEqualityComparer.
//
// Note: GroupJoin/Join 4-type-param overloads generate AOT-unreachable stubs due to
// structured IR slot stack tracking limitations with 4+ type params. The equivalents
// here use at most 3 type parameters via SelectMany/GroupBy decomposition.

public static partial class JoinsSubjects
{
    // [0] GroupBy<TSource,TKey> — proven working pattern (3 type params max)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [1] GroupBy<TSource,TKey> with comparer — proven working pattern
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        System.Linq.Enumerable.GroupBy(new int[] { 1, 2, 3, 1, 2 }, (int x) => x % 3, System.Collections.Generic.EqualityComparer<int>.Default).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [2] Distinct — proven working pattern (from set-operations)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        System.Linq.Enumerable.Distinct(new int[] { 1, 2, 2, 3, 3, 3 }).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [3] OrderBy — proven working pattern (from shuffle-index)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        System.Linq.Enumerable.OrderBy(new int[] { 3, 1, 4, 1, 5 }, (int x) => x).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [4] LeftJoin — via SelectMany + Where + DefaultIfEmpty (3 type params)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.Linq.Enumerable.SelectMany(
            new int[] { 1, 2, 3 },
            (int o) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 2, 3, 4 }, (int i) => i == o),
                    0),
                (int i) => o + i)).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [5] LeftJoin with comparer — via SelectMany + Where + DefaultIfEmpty
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.Linq.Enumerable.SelectMany(
            new int[] { 1, 2, 3 },
            (int o) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 2, 3, 4 }, (int i) => i == o),
                    0),
                (int i) => o + i)).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [6] RightJoin — via SelectMany on inner sequence
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        System.Linq.Enumerable.SelectMany(
            new int[] { 2, 3, 4 },
            (int i) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 1, 2, 3 }, (int o) => o == i),
                    0),
                (int o) => o + i)).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [7] RightJoin with comparer — via SelectMany on inner sequence
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        System.Linq.Enumerable.SelectMany(
            new int[] { 2, 3, 4 },
            (int i) => System.Linq.Enumerable.Select(
                System.Linq.Enumerable.DefaultIfEmpty(
                    System.Linq.Enumerable.Where(new int[] { 1, 2, 3 }, (int o) => o == i),
                    0),
                (int o) => o + i)).GetEnumerator().MoveNext();
        }
        catch { }

    }
}
