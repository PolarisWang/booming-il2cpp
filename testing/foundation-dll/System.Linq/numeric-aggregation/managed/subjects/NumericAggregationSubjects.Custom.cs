// Handwritten custom subject implementations for LINQ Numeric Aggregation operators.
//
// This family covers the ~90 overloads of Average, Sum, Min, Max, MinBy, and MaxBy
// on numeric types (int, long, float, double, decimal) and their nullable variants,
// with and without selector lambdas.
//
// All methods call System.Linq.Enumerable.MethodName(...) directly and discard the
// return value. The purpose is to exercise each overload through the il2cpp translation
// pipeline — the return values are verified by the verification framework separately.
//
// This file is a partial class of the auto-generated NumericAggregationSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.TestFramework;

public static partial class NumericAggregationSubjects
{
    // ========================================================================
    // AVERAGE — Base non-nullable overloads (indices 0-4)
    // ========================================================================

    // [0] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int32})
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3, 4, 5 });
        }
        catch { }

    }

    // [1] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int64})
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        Enumerable.Average(new long[] { 10L, 20L, 30L });
        }
        catch { }

    }

    // [2] System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        Enumerable.Average(new float[] { 1.5f, 2.5f, 3.5f });
        }
        catch { }

    }

    // [3] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        Enumerable.Average(new double[] { 1.5, 2.5, 3.5 });
        }
        catch { }

    }

    // [4] System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        Enumerable.Average(new decimal[] { 1.5m, 2.5m, 3.5m });
        }
        catch { }

    }

    // ========================================================================
    // AVERAGE — Base nullable overloads (indices 5-9)
    // ========================================================================

    // [5] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        Enumerable.Average(new int?[] { 1, null, 3, null, 5 });
        }
        catch { }

    }

    // [6] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        Enumerable.Average(new long?[] { 10L, null, 30L });
        }
        catch { }

    }

    // [7] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        Enumerable.Average(new float?[] { 1.5f, null, 3.5f });
        }
        catch { }

    }

    // [8] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        Enumerable.Average(new double?[] { 1.5, null, 3.5 });
        }
        catch { }

    }

    // [9] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        Enumerable.Average(new decimal?[] { 1.5m, null, 3.5m });
        }
        catch { }

    }

    // ========================================================================
    // AVERAGE — Selector non-nullable overloads (indices 10-14)
    // ========================================================================

    // [10] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    [Fact]
    public static void CustomEntrySubject_10()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x);
        }
        catch { }

    }

    // [11] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    [Fact]
    public static void CustomEntrySubject_11()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (long)x);
        }
        catch { }

    }

    // [12] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    [Fact]
    public static void CustomEntrySubject_12()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (float)x);
        }
        catch { }

    }

    // [13] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    [Fact]
    public static void CustomEntrySubject_13()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (double)x);
        }
        catch { }

    }

    // [14] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    [Fact]
    public static void CustomEntrySubject_14()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
        }
        catch { }

    }

    // ========================================================================
    // AVERAGE — Selector nullable overloads (indices 15-19)
    // ========================================================================

    // [15] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_15()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (int?)x : null);
        }
        catch { }

    }

    // [16] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_16()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (long?)x : null);
        }
        catch { }

    }

    // [17] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_17()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
        }
        catch { }

    }

    // [18] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_18()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
        }
        catch { }

    }

    // [19] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_19()
    {
        try
        {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
        }
        catch { }

    }

    // ========================================================================
    // MAX — Base overloads (indices 20-29)
    // ========================================================================

    // [20] System.Linq/System.Linq.Enumerable::Max:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    [Fact]
    public static void CustomEntrySubject_20()
    {
        try
        {
        Enumerable.Max(new int[] { 3, 1, 4, 1, 5, 9 });
        }
        catch { }

    }

    // [21] System.Linq/System.Linq.Enumerable::Max:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    [Fact]
    public static void CustomEntrySubject_21()
    {
        try
        {
        Enumerable.Max(new long[] { 10L, 30L, 20L });
        }
        catch { }

    }

    // [22] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_22()
    {
        try
        {
        Enumerable.Max(new int?[] { 3, null, 9, 1 });
        }
        catch { }

    }

    // [23] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_23()
    {
        try
        {
        Enumerable.Max(new long?[] { 10L, null, 30L });
        }
        catch { }

    }

    // [24] System.Linq/System.Linq.Enumerable::Max:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    [Fact]
    public static void CustomEntrySubject_24()
    {
        try
        {
        Enumerable.Max(new double[] { 1.5, 3.5, 2.5 });
        }
        catch { }

    }

    // [25] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_25()
    {
        try
        {
        Enumerable.Max(new double?[] { 1.5, null, 3.5 });
        }
        catch { }

    }

    // [26] System.Linq/System.Linq.Enumerable::Max:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    [Fact]
    public static void CustomEntrySubject_26()
    {
        try
        {
        Enumerable.Max(new float[] { 1.5f, 3.5f, 2.5f });
        }
        catch { }

    }

    // [27] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_27()
    {
        try
        {
        Enumerable.Max(new float?[] { 1.5f, null, 3.5f });
        }
        catch { }

    }

    // [28] System.Linq/System.Linq.Enumerable::Max:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    [Fact]
    public static void CustomEntrySubject_28()
    {
        try
        {
        Enumerable.Max(new decimal[] { 1.5m, 3.5m, 2.5m });
        }
        catch { }

    }

    // [29] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_29()
    {
        try
        {
        Enumerable.Max(new decimal?[] { 1.5m, null, 3.5m });
        }
        catch { }

    }

    // ========================================================================
    // MAX — Generic and comparer overloads (indices 30-31)
    // ========================================================================

    // [30] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})
    [Fact]
    public static void CustomEntrySubject_30()
    {
        try
        {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" });
        }
        catch { }

    }

    // [31] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})
    [Fact]
    public static void CustomEntrySubject_31()
    {
        try
        {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, StringComparer.OrdinalIgnoreCase);
        }
        catch { }

    }

    // ========================================================================
    // MAX — Selector overloads (indices 32-42)
    // ========================================================================

    // [32] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    [Fact]
    public static void CustomEntrySubject_32()
    {
        try
        {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
        }
        catch { }

    }

    // [33] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_33()
    {
        try
        {
        Enumerable.Max(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (int?)s.Length : null);
        }
        catch { }

    }

    // [34] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    [Fact]
    public static void CustomEntrySubject_34()
    {
        try
        {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, (string s) => (long)s.Length);
        }
        catch { }

    }

    // [35] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_35()
    {
        try
        {
        Enumerable.Max(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (long?)s.Length : null);
        }
        catch { }

    }

    // [36] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    [Fact]
    public static void CustomEntrySubject_36()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (float)x);
        }
        catch { }

    }

    // [37] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_37()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
        }
        catch { }

    }

    // [38] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    [Fact]
    public static void CustomEntrySubject_38()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (double)x);
        }
        catch { }

    }

    // [39] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_39()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
        }
        catch { }

    }

    // [40] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    [Fact]
    public static void CustomEntrySubject_40()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
        }
        catch { }

    }

    // [41] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_41()
    {
        try
        {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
        }
        catch { }

    }

    // [42] System.Linq/System.Linq.Enumerable::Max{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})
    [Fact]
    public static void CustomEntrySubject_42()
    {
        try
        {
        Enumerable.Max<string, string>(new string[] { "x", "yy", "zzz" }, (string s) => s);
        }
        catch { }

    }

    // ========================================================================
    // MAXBY — (indices 43-44)
    // ========================================================================

    // [43] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    [Fact]
    public static void CustomEntrySubject_43()
    {
        try
        {
        Enumerable.MaxBy(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
        }
        catch { }

    }

    // [44] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    [Fact]
    public static void CustomEntrySubject_44()
    {
        try
        {
        Enumerable.MaxBy(new string[] { "apple", "banana", "cherry" }, (string s) => s, StringComparer.OrdinalIgnoreCase);
        }
        catch { }

    }

    // ========================================================================
    // MIN — Base overloads (indices 45-54)
    // ========================================================================

    // [45] System.Linq/System.Linq.Enumerable::Min:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    [Fact]
    public static void CustomEntrySubject_45()
    {
        try
        {
        Enumerable.Min(new int[] { 3, 1, 4, 1, 5, 9 });
        }
        catch { }

    }

    // [46] System.Linq/System.Linq.Enumerable::Min:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    [Fact]
    public static void CustomEntrySubject_46()
    {
        try
        {
        Enumerable.Min(new long[] { 30L, 10L, 20L });
        }
        catch { }

    }

    // [47] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_47()
    {
        try
        {
        Enumerable.Min(new int?[] { 3, null, 1, 9 });
        }
        catch { }

    }

    // [48] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_48()
    {
        try
        {
        Enumerable.Min(new long?[] { 30L, null, 10L });
        }
        catch { }

    }

    // [49] System.Linq/System.Linq.Enumerable::Min:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    [Fact]
    public static void CustomEntrySubject_49()
    {
        try
        {
        Enumerable.Min(new float[] { 3.5f, 1.5f, 2.5f });
        }
        catch { }

    }

    // [50] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_50()
    {
        try
        {
        Enumerable.Min(new float?[] { 3.5f, null, 1.5f });
        }
        catch { }

    }

    // [51] System.Linq/System.Linq.Enumerable::Min:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    [Fact]
    public static void CustomEntrySubject_51()
    {
        try
        {
        Enumerable.Min(new double[] { 3.5, 1.5, 2.5 });
        }
        catch { }

    }

    // [52] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_52()
    {
        try
        {
        Enumerable.Min(new double?[] { 3.5, null, 1.5 });
        }
        catch { }

    }

    // [53] System.Linq/System.Linq.Enumerable::Min:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    [Fact]
    public static void CustomEntrySubject_53()
    {
        try
        {
        Enumerable.Min(new decimal[] { 3.5m, 1.5m, 2.5m });
        }
        catch { }

    }

    // [54] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_54()
    {
        try
        {
        Enumerable.Min(new decimal?[] { 3.5m, null, 1.5m });
        }
        catch { }

    }

    // ========================================================================
    // MIN — Generic and comparer overloads (indices 55-56)
    // ========================================================================

    // [55] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})
    [Fact]
    public static void CustomEntrySubject_55()
    {
        try
        {
        Enumerable.Min(new string[] { "cherry", "apple", "banana" });
        }
        catch { }

    }

    // [56] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})
    [Fact]
    public static void CustomEntrySubject_56()
    {
        try
        {
        Enumerable.Min(new string[] { "cherry", "apple", "banana" }, StringComparer.OrdinalIgnoreCase);
        }
        catch { }

    }

    // ========================================================================
    // MIN — Selector overloads (indices 57-67)
    // ========================================================================

    // [57] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    [Fact]
    public static void CustomEntrySubject_57()
    {
        try
        {
        Enumerable.Min(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
        }
        catch { }

    }

    // [58] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_58()
    {
        try
        {
        Enumerable.Min(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (int?)s.Length : null);
        }
        catch { }

    }

    // [59] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    [Fact]
    public static void CustomEntrySubject_59()
    {
        try
        {
        Enumerable.Min(new string[] { "apple", "banana", "cherry" }, (string s) => (long)s.Length);
        }
        catch { }

    }

    // [60] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_60()
    {
        try
        {
        Enumerable.Min(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (long?)s.Length : null);
        }
        catch { }

    }

    // [61] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    [Fact]
    public static void CustomEntrySubject_61()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (float)x);
        }
        catch { }

    }

    // [62] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_62()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
        }
        catch { }

    }

    // [63] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    [Fact]
    public static void CustomEntrySubject_63()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (double)x);
        }
        catch { }

    }

    // [64] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_64()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
        }
        catch { }

    }

    // [65] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    [Fact]
    public static void CustomEntrySubject_65()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
        }
        catch { }

    }

    // [66] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_66()
    {
        try
        {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
        }
        catch { }

    }

    // [67] System.Linq/System.Linq.Enumerable::Min{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})
    [Fact]
    public static void CustomEntrySubject_67()
    {
        try
        {
        Enumerable.Min<string, string>(new string[] { "zzz", "yy", "x" }, (string s) => s);
        }
        catch { }

    }

    // ========================================================================
    // MINBY — (indices 68-69)
    // ========================================================================

    // [68] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    [Fact]
    public static void CustomEntrySubject_68()
    {
        try
        {
        Enumerable.MinBy(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
        }
        catch { }

    }

    // [69] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    [Fact]
    public static void CustomEntrySubject_69()
    {
        try
        {
        Enumerable.MinBy(new string[] { "apple", "banana", "cherry" }, (string s) => s, StringComparer.OrdinalIgnoreCase);
        }
        catch { }

    }

    // ========================================================================
    // SUM — Base non-nullable overloads (indices 70-74)
    // ========================================================================

    // [70] System.Linq/System.Linq.Enumerable::Sum:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    [Fact]
    public static void CustomEntrySubject_70()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3, 4, 5 });
        }
        catch { }

    }

    // [71] System.Linq/System.Linq.Enumerable::Sum:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    [Fact]
    public static void CustomEntrySubject_71()
    {
        try
        {
        Enumerable.Sum(new long[] { 10L, 20L, 30L });
        }
        catch { }

    }

    // [72] System.Linq/System.Linq.Enumerable::Sum:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    [Fact]
    public static void CustomEntrySubject_72()
    {
        try
        {
        Enumerable.Sum(new float[] { 1.5f, 2.5f, 3.5f });
        }
        catch { }

    }

    // [73] System.Linq/System.Linq.Enumerable::Sum:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    [Fact]
    public static void CustomEntrySubject_73()
    {
        try
        {
        Enumerable.Sum(new double[] { 1.5, 2.5, 3.5 });
        }
        catch { }

    }

    // [74] System.Linq/System.Linq.Enumerable::Sum:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    [Fact]
    public static void CustomEntrySubject_74()
    {
        try
        {
        Enumerable.Sum(new decimal[] { 1.5m, 2.5m, 3.5m });
        }
        catch { }

    }

    // ========================================================================
    // SUM — Base nullable overloads (indices 75-79)
    // ========================================================================

    // [75] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_75()
    {
        try
        {
        Enumerable.Sum(new int?[] { 1, null, 3, null, 5 });
        }
        catch { }

    }

    // [76] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_76()
    {
        try
        {
        Enumerable.Sum(new long?[] { 10L, null, 30L });
        }
        catch { }

    }

    // [77] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_77()
    {
        try
        {
        Enumerable.Sum(new float?[] { 1.5f, null, 3.5f });
        }
        catch { }

    }

    // [78] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_78()
    {
        try
        {
        Enumerable.Sum(new double?[] { 1.5, null, 3.5 });
        }
        catch { }

    }

    // [79] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_79()
    {
        try
        {
        Enumerable.Sum(new decimal?[] { 1.5m, null, 3.5m });
        }
        catch { }

    }

    // ========================================================================
    // SUM — Selector non-nullable overloads (indices 80-84)
    // ========================================================================

    // [80] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    [Fact]
    public static void CustomEntrySubject_80()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x);
        }
        catch { }

    }

    // [81] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    [Fact]
    public static void CustomEntrySubject_81()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (long)x);
        }
        catch { }

    }

    // [82] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    [Fact]
    public static void CustomEntrySubject_82()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (float)x);
        }
        catch { }

    }

    // [83] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    [Fact]
    public static void CustomEntrySubject_83()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (double)x);
        }
        catch { }

    }

    // [84] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    [Fact]
    public static void CustomEntrySubject_84()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
        }
        catch { }

    }

    // ========================================================================
    // SUM — Selector nullable overloads (indices 85-89)
    // ========================================================================

    // [85] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    [Fact]
    public static void CustomEntrySubject_85()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (int?)x : null);
        }
        catch { }

    }

    // [86] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    [Fact]
    public static void CustomEntrySubject_86()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (long?)x : null);
        }
        catch { }

    }

    // [87] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    [Fact]
    public static void CustomEntrySubject_87()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
        }
        catch { }

    }

    // [88] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    [Fact]
    public static void CustomEntrySubject_88()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
        }
        catch { }

    }

    // [89] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    [Fact]
    public static void CustomEntrySubject_89()
    {
        try
        {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
        }
        catch { }

    }
}
