// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/collections/generic/core
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class CollectionsGenericCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    }

    // [1] System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
    public static void Subject_1()
    {
        try { new List<int>().Clear(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    public static void Subject_2()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    }

    // [3] System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    }

    // [4] System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
    public static void Subject_4()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
    }

    // [5] System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)
    public static void Subject_5()
    {
        try { new System.Collections.Generic.List<int>{1,2,3}.RemoveAt(1); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()
    public static void Subject_6()
    {
        try { new List<int>().Sort(); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()
    public static void Subject_7()
    {
        try { if (((new List<int>().ToArray()).Length) != ((new List<int>().ToArray()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    public static void Subject_8()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    }

    // [9] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
    public static void Subject_9()
    {
        try { if (new Dictionary<string, int>().Count != new Dictionary<string, int>().Count) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    public static void Subject_10()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    }

    // [11] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    public static void Subject_11()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    }

    // [12] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    public static void Subject_12()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    }

    // [13] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    public static void Subject_13()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    }

    // [14] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    public static void Subject_14()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    }

    // [15] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
    public static void Subject_15()
    {
        // non-callable: System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
    }

}