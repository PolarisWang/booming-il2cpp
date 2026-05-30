// Handwritten custom entry implementations for Generic Collections Core.
//
// All methods involve generic types (List<T>, Dictionary<K,V>, HashSet<T>)
// that the auto-generator cannot emit for parameter construction.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System.Collections.Generic;
using Chaos.TestFramework;

public static partial class CollectionsGenericCoreNativeEntry
{
    // [0] System.Collections.Generic.List`1::Add(T)
    [Fact]
    public static void CustomEntryMethod0()
    {
        new List<int>().Add(42);
    }

    // [2] System.Collections.Generic.List`1::Contains(T)
    [Fact]
    public static void CustomEntryMethod2()
    {
        var list = new List<int> { 1, 2, 3 };
        Assert.IsTrue(list.Contains(2));
    }

    // [3] System.Collections.Generic.List`1::IndexOf(T)
    [Fact]
    public static void CustomEntryMethod3()
    {
        var list = new List<int> { 1, 2, 3 };
        Assert.AreEqual(1, list.IndexOf(2));
    }

    // [4] System.Collections.Generic.List`1::Remove(T)
    [Fact]
    public static void CustomEntryMethod4()
    {
        var list = new List<int> { 1, 2, 3 };
        list.Remove(2);
    }

    // [8] System.Collections.Generic.Dictionary`2::Add(TKey,TValue)
    [Fact]
    public static void CustomEntryMethod8()
    {
        new Dictionary<string, int>().Add("a", 1);
    }

    // [10] System.Collections.Generic.Dictionary`2::TryGetValue(TKey,TValue&)
    [Fact]
    public static void CustomEntryMethod10()
    {
        var dict = new Dictionary<string, int> { { "a", 42 } };
        Assert.IsTrue(dict.TryGetValue("a", out int v));
    }

    // [11] System.Collections.Generic.Dictionary`2::ContainsKey(TKey)
    [Fact]
    public static void CustomEntryMethod11()
    {
        var dict = new Dictionary<string, int> { { "a", 1 } };
        Assert.IsTrue(dict.ContainsKey("a"));
    }

    // [12] System.Collections.Generic.Dictionary`2::Remove(TKey)
    [Fact]
    public static void CustomEntryMethod12()
    {
        var dict = new Dictionary<string, int> { { "a", 1 } };
        dict.Remove("a");
    }

    // [13] System.Collections.Generic.HashSet`1::Add(T)
    [Fact]
    public static void CustomEntryMethod13()
    {
        new HashSet<int>().Add(42);
    }

    // [14] System.Collections.Generic.HashSet`1::Contains(T)
    [Fact]
    public static void CustomEntryMethod14()
    {
        var set = new HashSet<int> { 42 };
        Assert.IsTrue(set.Contains(42));
    }

    // [15] System.Collections.Generic.HashSet`1::Remove(T)
    [Fact]
    public static void CustomEntryMethod15()
    {
        var set = new HashSet<int> { 42 };
        set.Remove(42);
    }
}
