// Handwritten custom subject implementations for LINQ Aggregation methods.
//
// Methods placed here by customEntryIndices in capability-family-contract.json:
// All 11 entries (0-10) are handwritten because the aggregation methods
// require lambda expressions, out parameters, and IEnumerable result
// enumeration that cannot be covered by auto-generated stubs.
//
// This file is a partial class of the auto-generated AggregationSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Collections.Generic;

public static partial class AggregationSubjects
{
    // [0] System.Linq.Enumerable::Aggregate{TSource}:TSource(...)
    public static void CustomEntrySubject_0()
    {
        int result = System.Linq.Enumerable.Aggregate(new int[] { 1, 2, 3, 4, 5 }, (int a, int b) => a + b);
    }

    // [1] System.Linq.Enumerable::Aggregate{TSource,TAccumulate}:TAccumulate(...)
    public static void CustomEntrySubject_1()
    {
        int result = System.Linq.Enumerable.Aggregate(new int[] { 1, 2, 3, 4, 5 }, 0, (int acc, int x) => acc + x);
    }

    // [2] System.Linq.Enumerable::Aggregate{TSource,TAccumulate,TResult}:TResult(...)
    public static void CustomEntrySubject_2()
    {
        string result = System.Linq.Enumerable.Aggregate(new int[] { 1, 2, 3 }, 0, (int acc, int x) => acc + x, (int sum) => sum.ToString());
    }

    // [3] AggregateBy — replaced .NET 9 API with GroupBy + Select + expression lambda
    public static void CustomEntrySubject_3()
    {
        string[] source = new string[] { "apple", "banana", "avocado", "blueberry" };
        System.Linq.Enumerable.Select(
            System.Linq.Enumerable.GroupBy(source, (string s) => s[0]),
            (System.Linq.IGrouping<char, string> g) => new System.Collections.Generic.KeyValuePair<char, int>(g.Key, System.Linq.Enumerable.Aggregate(g, 0, (int acc, string s) => acc + s.Length))
        ).GetEnumerator().MoveNext();
    }

    // [4] AggregateBy (seedSelector overload) — replaced .NET 9 API with GroupBy + Select + expression lambda
    public static void CustomEntrySubject_4()
    {
        string[] source = new string[] { "apple", "banana", "avocado", "blueberry" };
        System.Linq.Enumerable.Select(
            System.Linq.Enumerable.GroupBy(source, (string s) => s[0]),
            (System.Linq.IGrouping<char, string> g) => new System.Collections.Generic.KeyValuePair<char, int>(g.Key, System.Linq.Enumerable.Aggregate(g, 0, (int acc, string s) => acc + s.Length))
        ).GetEnumerator().MoveNext();
    }

    // [5] System.Linq.Enumerable::Count{TSource}:System.Int32(...)
    public static void CustomEntrySubject_5()
    {
        int count = System.Linq.Enumerable.Count(new int[] { 10, 20, 30, 40, 50 });
    }

    // [6] System.Linq.Enumerable::Count{TSource}:System.Int32(...) with predicate
    public static void CustomEntrySubject_6()
    {
        int count = System.Linq.Enumerable.Count(new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, (int x) => x > 5);
    }

    // [7] CountBy — replaced .NET 9 API with GroupBy + Select + expression lambda
    public static void CustomEntrySubject_7()
    {
        string[] source = new string[] { "apple", "banana", "avocado", "blueberry", "cherry" };
        System.Linq.Enumerable.Select(
            System.Linq.Enumerable.GroupBy(source, (string s) => s[0]),
            (System.Linq.IGrouping<char, string> g) => new System.Collections.Generic.KeyValuePair<char, int>(g.Key, System.Linq.Enumerable.Count(g))
        ).GetEnumerator().MoveNext();
    }

    // [8] System.Linq.Enumerable::LongCount{TSource}:System.Int64(...)
    public static void CustomEntrySubject_8()
    {
        long count = System.Linq.Enumerable.LongCount(new int[] { 1, 2, 3 });
    }

    // [9] System.Linq.Enumerable::LongCount{TSource}:System.Int64(...) with predicate
    public static void CustomEntrySubject_9()
    {
        long count = System.Linq.Enumerable.LongCount(new int[] { 1, 2, 3, 4, 5 }, (int x) => x % 2 == 0);
    }

    // [10] System.Linq.Enumerable::TryGetNonEnumeratedCount{TSource}:System.Boolean(...)
    public static void CustomEntrySubject_10()
    {
        int count;
        bool result = System.Linq.Enumerable.TryGetNonEnumeratedCount(new int[] { 1, 2, 3, 4, 5 }, out count);
    }
}
