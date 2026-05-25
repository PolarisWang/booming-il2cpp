// Handwritten custom entry implementations for Generic Collections Core.
//
// All methods involve generic types (List<T>, Dictionary<K,V>, HashSet<T>)
// that the auto-generator cannot emit for parameter construction.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System.Collections.Generic;

public static partial class CollectionsGenericCoreNativeEntry
{
    public static int _exitCode;

    // [0] System.Collections.Generic.List`1::Add(T)
    public static void CustomEntryMethod0()
    {
        new List<int>().Add(42);
    }

    // [2] System.Collections.Generic.List`1::Contains(T)
    public static void CustomEntryMethod2()
    {
        var list = new List<int> { 1, 2, 3 };
        if (!list.Contains(2)) _exitCode = 1;
    }

    // [3] System.Collections.Generic.List`1::IndexOf(T)
    public static void CustomEntryMethod3()
    {
        var list = new List<int> { 1, 2, 3 };
        if (list.IndexOf(2) != 1) _exitCode = 1;
    }

    // [4] System.Collections.Generic.List`1::Remove(T)
    public static void CustomEntryMethod4()
    {
        var list = new List<int> { 1, 2, 3 };
        list.Remove(2);
    }

    // [8] System.Collections.Generic.Dictionary`2::Add(TKey,TValue)
    public static void CustomEntryMethod8()
    {
        new Dictionary<string, int>().Add("a", 1);
    }

    // [10] System.Collections.Generic.Dictionary`2::TryGetValue(TKey,TValue&)
    public static void CustomEntryMethod10()
    {
        var dict = new Dictionary<string, int> { { "a", 42 } };
        if (!dict.TryGetValue("a", out int v)) _exitCode = 1;
    }

    // [11] System.Collections.Generic.Dictionary`2::ContainsKey(TKey)
    public static void CustomEntryMethod11()
    {
        var dict = new Dictionary<string, int> { { "a", 1 } };
        if (!dict.ContainsKey("a")) _exitCode = 1;
    }

    // [12] System.Collections.Generic.Dictionary`2::Remove(TKey)
    public static void CustomEntryMethod12()
    {
        var dict = new Dictionary<string, int> { { "a", 1 } };
        dict.Remove("a");
    }

    // [13] System.Collections.Generic.HashSet`1::Add(T)
    public static void CustomEntryMethod13()
    {
        new HashSet<int>().Add(42);
    }

    // [14] System.Collections.Generic.HashSet`1::Contains(T)
    public static void CustomEntryMethod14()
    {
        var set = new HashSet<int> { 42 };
        if (!set.Contains(42)) _exitCode = 1;
    }

    // [15] System.Collections.Generic.HashSet`1::Remove(T)
    public static void CustomEntryMethod15()
    {
        var set = new HashSet<int> { 42 };
        set.Remove(42);
    }
}
