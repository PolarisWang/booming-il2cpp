// Handwritten custom entry implementations for System.Linq materialization operations.
// Covers AsEnumerable, ToArray, ToDictionary (9 overloads), ToHashSet (2 overloads),
// ToList, and ToLookup (4 overloads) -- all terminal operations that materialize
// a sequence into a concrete collection or structure.
// Each method calls the corresponding Enumerable API with appropriate test data.
// Terminal operations (ToArray/ToList/ToDictionary/ToHashSet/ToLookup) complete
// evaluation upon return; AsEnumerable is forced via IEnumerator<T>.

public static partial class MaterializationSubjects
{
    // [0] Enumerable.AsEnumerable<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_0()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3 };
        System.Collections.Generic.IEnumerable<int> result = System.Linq.Enumerable.AsEnumerable(source);
        System.Collections.Generic.IEnumerator<int> e = result.GetEnumerator();
        e.MoveNext();
        }
        catch { _exitCode = 1; }

    }

    // [1] Enumerable.ToArray<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_1()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        int[] result = System.Linq.Enumerable.ToArray(source);
        }
        catch { _exitCode = 1; }

    }

    // [2] Enumerable.ToDictionary<TKey,TValue>(IEnumerable<KeyValuePair<TKey,TValue>>)
    public static void CustomEntrySubject_2()
    {
        try
        {
        System.Collections.Generic.KeyValuePair<int, string>[] source = new System.Collections.Generic.KeyValuePair<int, string>[]
        {
            new System.Collections.Generic.KeyValuePair<int, string>(1, "one"),
            new System.Collections.Generic.KeyValuePair<int, string>(2, "two"),
            new System.Collections.Generic.KeyValuePair<int, string>(3, "three")
        }
        catch { _exitCode = 1; }

        };
        System.Collections.Generic.Dictionary<int, string> result = System.Linq.Enumerable.ToDictionary(source);
    }

    // [3] Enumerable.ToDictionary<TKey,TValue>(IEnumerable<KeyValuePair<TKey,TValue>>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_3()
    {
        try
        {
        System.Collections.Generic.KeyValuePair<int, string>[] source = new System.Collections.Generic.KeyValuePair<int, string>[]
        {
            new System.Collections.Generic.KeyValuePair<int, string>(1, "one"),
            new System.Collections.Generic.KeyValuePair<int, string>(2, "two")
        }
        catch { _exitCode = 1; }

        };
        System.Collections.Generic.Dictionary<int, string> result = System.Linq.Enumerable.ToDictionary(source, System.Collections.Generic.EqualityComparer<int>.Default);
    }

    // [4] Enumerable.ToDictionary<TKey,TValue>(IEnumerable<ValueTuple<TKey,TValue>>)
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.ValueTuple<int, string>[] source = new System.ValueTuple<int, string>[]
        {
            new System.ValueTuple<int, string>(1, "one"),
            new System.ValueTuple<int, string>(2, "two"),
            new System.ValueTuple<int, string>(3, "three")
        }
        catch { _exitCode = 1; }

        };
        System.Collections.Generic.Dictionary<int, string> result = System.Linq.Enumerable.ToDictionary(source);
    }

    // [5] Enumerable.ToDictionary<TKey,TValue>(IEnumerable<ValueTuple<TKey,TValue>>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.ValueTuple<int, string>[] source = new System.ValueTuple<int, string>[]
        {
            new System.ValueTuple<int, string>(1, "one"),
            new System.ValueTuple<int, string>(2, "two")
        }
        catch { _exitCode = 1; }

        };
        System.Collections.Generic.Dictionary<int, string> result = System.Linq.Enumerable.ToDictionary(source, System.Collections.Generic.EqualityComparer<int>.Default);
    }

    // [6] Enumerable.ToDictionary<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_6()
    {
        try
        {
        string[] source = new string[] { "one", "two", "three" };
        System.Collections.Generic.Dictionary<char, string> result = System.Linq.Enumerable.ToDictionary(source, (string s) => s[0]);
        }
        catch { _exitCode = 1; }

    }

    // [7] Enumerable.ToDictionary<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_7()
    {
        try
        {
        string[] source = new string[] { "one", "two", "three" };
        System.Collections.Generic.Dictionary<char, string> result = System.Linq.Enumerable.ToDictionary(source, (string s) => s[0], System.Collections.Generic.EqualityComparer<char>.Default);
        }
        catch { _exitCode = 1; }

    }

    // [8] Enumerable.ToDictionary<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>)
    public static void CustomEntrySubject_8()
    {
        try
        {
        string[] source = new string[] { "one", "two", "three" };
        System.Collections.Generic.Dictionary<char, int> result = System.Linq.Enumerable.ToDictionary(source, (string s) => s[0], (string s) => s.Length);
        }
        catch { _exitCode = 1; }

    }

    // [9] Enumerable.ToDictionary<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_9()
    {
        try
        {
        string[] source = new string[] { "one", "two", "three" };
        System.Collections.Generic.Dictionary<char, int> result = System.Linq.Enumerable.ToDictionary(source, (string s) => s[0], (string s) => s.Length, System.Collections.Generic.EqualityComparer<char>.Default);
        }
        catch { _exitCode = 1; }

    }

    // [10] Enumerable.ToHashSet<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_10()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 2, 1 };
        System.Collections.Generic.HashSet<int> result = System.Linq.Enumerable.ToHashSet(source);
        }
        catch { _exitCode = 1; }

    }

    // [11] Enumerable.ToHashSet<TSource>(IEnumerable<TSource>, IEqualityComparer<TSource>)
    public static void CustomEntrySubject_11()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 2, 1 };
        System.Collections.Generic.HashSet<int> result = System.Linq.Enumerable.ToHashSet(source, System.Collections.Generic.EqualityComparer<int>.Default);
        }
        catch { _exitCode = 1; }

    }

    // [12] Enumerable.ToList<TSource>(IEnumerable<TSource>)
    public static void CustomEntrySubject_12()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Collections.Generic.List<int> result = System.Linq.Enumerable.ToList(source);
        }
        catch { _exitCode = 1; }

    }

    // [13] Enumerable.ToLookup<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>)
    public static void CustomEntrySubject_13()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.ILookup<int, int> result = System.Linq.Enumerable.ToLookup(source, (int x) => x % 2);
        }
        catch { _exitCode = 1; }

    }

    // [14] Enumerable.ToLookup<TSource,TKey>(IEnumerable<TSource>, Func<TSource,TKey>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_14()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.ILookup<int, int> result = System.Linq.Enumerable.ToLookup(source, (int x) => x % 2, System.Collections.Generic.EqualityComparer<int>.Default);
        }
        catch { _exitCode = 1; }

    }

    // [15] Enumerable.ToLookup<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>)
    public static void CustomEntrySubject_15()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.ILookup<int, bool> result = System.Linq.Enumerable.ToLookup(source, (int x) => x % 2, (int x) => x > 2);
        }
        catch { _exitCode = 1; }

    }

    // [16] Enumerable.ToLookup<TSource,TKey,TElement>(IEnumerable<TSource>, Func<TSource,TKey>, Func<TSource,TElement>, IEqualityComparer<TKey>)
    public static void CustomEntrySubject_16()
    {
        try
        {
        int[] source = new int[] { 1, 2, 3, 4, 5 };
        System.Linq.ILookup<int, bool> result = System.Linq.Enumerable.ToLookup(source, (int x) => x % 2, (int x) => x > 2, System.Collections.Generic.EqualityComparer<int>.Default);
        }
        catch { _exitCode = 1; }

    }
}
