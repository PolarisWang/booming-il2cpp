// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/collections/generic/core
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class CollectionsGenericCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    // [1] System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
    public static void Subject_1()
    {
        try { new List<int>().Clear(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    // [3] System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    // [4] System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
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
        try { _ = ((new List<int>().ToArray()).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    // [9] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
    public static void Subject_9()
    {
        try { _ = new Dictionary<string, int>().Count; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    // [11] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    // [12] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    // [13] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    // [14] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    // [15] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
}