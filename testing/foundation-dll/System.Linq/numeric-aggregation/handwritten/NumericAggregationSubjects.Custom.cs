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

public static partial class NumericAggregationSubjects
{
    // ========================================================================
    // AVERAGE — Base non-nullable overloads (indices 0-4)
    // ========================================================================

    // [0] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int32})
    public static void CustomEntrySubject_0()
    {
        Enumerable.Average(new int[] { 1, 2, 3, 4, 5 });
    }

    // [1] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int64})
    public static void CustomEntrySubject_1()
    {
        Enumerable.Average(new long[] { 10L, 20L, 30L });
    }

    // [2] System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    public static void CustomEntrySubject_2()
    {
        Enumerable.Average(new float[] { 1.5f, 2.5f, 3.5f });
    }

    // [3] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    public static void CustomEntrySubject_3()
    {
        Enumerable.Average(new double[] { 1.5, 2.5, 3.5 });
    }

    // [4] System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    public static void CustomEntrySubject_4()
    {
        Enumerable.Average(new decimal[] { 1.5m, 2.5m, 3.5m });
    }

    // ========================================================================
    // AVERAGE — Base nullable overloads (indices 5-9)
    // ========================================================================

    // [5] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    public static void CustomEntrySubject_5()
    {
        Enumerable.Average(new int?[] { 1, null, 3, null, 5 });
    }

    // [6] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    public static void CustomEntrySubject_6()
    {
        Enumerable.Average(new long?[] { 10L, null, 30L });
    }

    // [7] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    public static void CustomEntrySubject_7()
    {
        Enumerable.Average(new float?[] { 1.5f, null, 3.5f });
    }

    // [8] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    public static void CustomEntrySubject_8()
    {
        Enumerable.Average(new double?[] { 1.5, null, 3.5 });
    }

    // [9] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_9()
    {
        Enumerable.Average(new decimal?[] { 1.5m, null, 3.5m });
    }

    // ========================================================================
    // AVERAGE — Selector non-nullable overloads (indices 10-14)
    // ========================================================================

    // [10] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    public static void CustomEntrySubject_10()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x);
    }

    // [11] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    public static void CustomEntrySubject_11()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (long)x);
    }

    // [12] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    public static void CustomEntrySubject_12()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (float)x);
    }

    // [13] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    public static void CustomEntrySubject_13()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (double)x);
    }

    // [14] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    public static void CustomEntrySubject_14()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
    }

    // ========================================================================
    // AVERAGE — Selector nullable overloads (indices 15-19)
    // ========================================================================

    // [15] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    public static void CustomEntrySubject_15()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (int?)x : null);
    }

    // [16] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    public static void CustomEntrySubject_16()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (long?)x : null);
    }

    // [17] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    public static void CustomEntrySubject_17()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
    }

    // [18] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    public static void CustomEntrySubject_18()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
    }

    // [19] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_19()
    {
        Enumerable.Average(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
    }

    // ========================================================================
    // MAX — Base overloads (indices 20-29)
    // ========================================================================

    // [20] System.Linq/System.Linq.Enumerable::Max:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    public static void CustomEntrySubject_20()
    {
        Enumerable.Max(new int[] { 3, 1, 4, 1, 5, 9 });
    }

    // [21] System.Linq/System.Linq.Enumerable::Max:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    public static void CustomEntrySubject_21()
    {
        Enumerable.Max(new long[] { 10L, 30L, 20L });
    }

    // [22] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    public static void CustomEntrySubject_22()
    {
        Enumerable.Max(new int?[] { 3, null, 9, 1 });
    }

    // [23] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    public static void CustomEntrySubject_23()
    {
        Enumerable.Max(new long?[] { 10L, null, 30L });
    }

    // [24] System.Linq/System.Linq.Enumerable::Max:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    public static void CustomEntrySubject_24()
    {
        Enumerable.Max(new double[] { 1.5, 3.5, 2.5 });
    }

    // [25] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    public static void CustomEntrySubject_25()
    {
        Enumerable.Max(new double?[] { 1.5, null, 3.5 });
    }

    // [26] System.Linq/System.Linq.Enumerable::Max:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    public static void CustomEntrySubject_26()
    {
        Enumerable.Max(new float[] { 1.5f, 3.5f, 2.5f });
    }

    // [27] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    public static void CustomEntrySubject_27()
    {
        Enumerable.Max(new float?[] { 1.5f, null, 3.5f });
    }

    // [28] System.Linq/System.Linq.Enumerable::Max:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    public static void CustomEntrySubject_28()
    {
        Enumerable.Max(new decimal[] { 1.5m, 3.5m, 2.5m });
    }

    // [29] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_29()
    {
        Enumerable.Max(new decimal?[] { 1.5m, null, 3.5m });
    }

    // ========================================================================
    // MAX — Generic and comparer overloads (indices 30-31)
    // ========================================================================

    // [30] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})
    public static void CustomEntrySubject_30()
    {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" });
    }

    // [31] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})
    public static void CustomEntrySubject_31()
    {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, StringComparer.OrdinalIgnoreCase);
    }

    // ========================================================================
    // MAX — Selector overloads (indices 32-42)
    // ========================================================================

    // [32] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    public static void CustomEntrySubject_32()
    {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
    }

    // [33] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    public static void CustomEntrySubject_33()
    {
        Enumerable.Max(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (int?)s.Length : null);
    }

    // [34] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    public static void CustomEntrySubject_34()
    {
        Enumerable.Max(new string[] { "apple", "banana", "cherry" }, (string s) => (long)s.Length);
    }

    // [35] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    public static void CustomEntrySubject_35()
    {
        Enumerable.Max(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (long?)s.Length : null);
    }

    // [36] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    public static void CustomEntrySubject_36()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (float)x);
    }

    // [37] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    public static void CustomEntrySubject_37()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
    }

    // [38] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    public static void CustomEntrySubject_38()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (double)x);
    }

    // [39] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    public static void CustomEntrySubject_39()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
    }

    // [40] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    public static void CustomEntrySubject_40()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
    }

    // [41] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_41()
    {
        Enumerable.Max(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
    }

    // [42] System.Linq/System.Linq.Enumerable::Max{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})
    public static void CustomEntrySubject_42()
    {
        Enumerable.Max<string, string>(new string[] { "x", "yy", "zzz" }, (string s) => s);
    }

    // ========================================================================
    // MAXBY — (indices 43-44)
    // ========================================================================

    // [43] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static void CustomEntrySubject_43()
    {
        Enumerable.MaxBy(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
    }

    // [44] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static void CustomEntrySubject_44()
    {
        Enumerable.MaxBy(new string[] { "apple", "banana", "cherry" }, (string s) => s, StringComparer.OrdinalIgnoreCase);
    }

    // ========================================================================
    // MIN — Base overloads (indices 45-54)
    // ========================================================================

    // [45] System.Linq/System.Linq.Enumerable::Min:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    public static void CustomEntrySubject_45()
    {
        Enumerable.Min(new int[] { 3, 1, 4, 1, 5, 9 });
    }

    // [46] System.Linq/System.Linq.Enumerable::Min:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    public static void CustomEntrySubject_46()
    {
        Enumerable.Min(new long[] { 30L, 10L, 20L });
    }

    // [47] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    public static void CustomEntrySubject_47()
    {
        Enumerable.Min(new int?[] { 3, null, 1, 9 });
    }

    // [48] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    public static void CustomEntrySubject_48()
    {
        Enumerable.Min(new long?[] { 30L, null, 10L });
    }

    // [49] System.Linq/System.Linq.Enumerable::Min:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    public static void CustomEntrySubject_49()
    {
        Enumerable.Min(new float[] { 3.5f, 1.5f, 2.5f });
    }

    // [50] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    public static void CustomEntrySubject_50()
    {
        Enumerable.Min(new float?[] { 3.5f, null, 1.5f });
    }

    // [51] System.Linq/System.Linq.Enumerable::Min:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    public static void CustomEntrySubject_51()
    {
        Enumerable.Min(new double[] { 3.5, 1.5, 2.5 });
    }

    // [52] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    public static void CustomEntrySubject_52()
    {
        Enumerable.Min(new double?[] { 3.5, null, 1.5 });
    }

    // [53] System.Linq/System.Linq.Enumerable::Min:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    public static void CustomEntrySubject_53()
    {
        Enumerable.Min(new decimal[] { 3.5m, 1.5m, 2.5m });
    }

    // [54] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_54()
    {
        Enumerable.Min(new decimal?[] { 3.5m, null, 1.5m });
    }

    // ========================================================================
    // MIN — Generic and comparer overloads (indices 55-56)
    // ========================================================================

    // [55] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})
    public static void CustomEntrySubject_55()
    {
        Enumerable.Min(new string[] { "cherry", "apple", "banana" });
    }

    // [56] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})
    public static void CustomEntrySubject_56()
    {
        Enumerable.Min(new string[] { "cherry", "apple", "banana" }, StringComparer.OrdinalIgnoreCase);
    }

    // ========================================================================
    // MIN — Selector overloads (indices 57-67)
    // ========================================================================

    // [57] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    public static void CustomEntrySubject_57()
    {
        Enumerable.Min(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
    }

    // [58] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    public static void CustomEntrySubject_58()
    {
        Enumerable.Min(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (int?)s.Length : null);
    }

    // [59] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    public static void CustomEntrySubject_59()
    {
        Enumerable.Min(new string[] { "apple", "banana", "cherry" }, (string s) => (long)s.Length);
    }

    // [60] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    public static void CustomEntrySubject_60()
    {
        Enumerable.Min(new string[] { "a", "bb", "ccc" }, (string s) => s.Length > 1 ? (long?)s.Length : null);
    }

    // [61] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    public static void CustomEntrySubject_61()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (float)x);
    }

    // [62] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    public static void CustomEntrySubject_62()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
    }

    // [63] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    public static void CustomEntrySubject_63()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (double)x);
    }

    // [64] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    public static void CustomEntrySubject_64()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
    }

    // [65] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    public static void CustomEntrySubject_65()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
    }

    // [66] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_66()
    {
        Enumerable.Min(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
    }

    // [67] System.Linq/System.Linq.Enumerable::Min{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})
    public static void CustomEntrySubject_67()
    {
        Enumerable.Min<string, string>(new string[] { "zzz", "yy", "x" }, (string s) => s);
    }

    // ========================================================================
    // MINBY — (indices 68-69)
    // ========================================================================

    // [68] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static void CustomEntrySubject_68()
    {
        Enumerable.MinBy(new string[] { "apple", "banana", "cherry" }, (string s) => s.Length);
    }

    // [69] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})
    public static void CustomEntrySubject_69()
    {
        Enumerable.MinBy(new string[] { "apple", "banana", "cherry" }, (string s) => s, StringComparer.OrdinalIgnoreCase);
    }

    // ========================================================================
    // SUM — Base non-nullable overloads (indices 70-74)
    // ========================================================================

    // [70] System.Linq/System.Linq.Enumerable::Sum:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})
    public static void CustomEntrySubject_70()
    {
        Enumerable.Sum(new int[] { 1, 2, 3, 4, 5 });
    }

    // [71] System.Linq/System.Linq.Enumerable::Sum:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})
    public static void CustomEntrySubject_71()
    {
        Enumerable.Sum(new long[] { 10L, 20L, 30L });
    }

    // [72] System.Linq/System.Linq.Enumerable::Sum:System.Single(System.Collections.Generic.IEnumerable{System.Single})
    public static void CustomEntrySubject_72()
    {
        Enumerable.Sum(new float[] { 1.5f, 2.5f, 3.5f });
    }

    // [73] System.Linq/System.Linq.Enumerable::Sum:System.Double(System.Collections.Generic.IEnumerable{System.Double})
    public static void CustomEntrySubject_73()
    {
        Enumerable.Sum(new double[] { 1.5, 2.5, 3.5 });
    }

    // [74] System.Linq/System.Linq.Enumerable::Sum:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})
    public static void CustomEntrySubject_74()
    {
        Enumerable.Sum(new decimal[] { 1.5m, 2.5m, 3.5m });
    }

    // ========================================================================
    // SUM — Base nullable overloads (indices 75-79)
    // ========================================================================

    // [75] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})
    public static void CustomEntrySubject_75()
    {
        Enumerable.Sum(new int?[] { 1, null, 3, null, 5 });
    }

    // [76] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})
    public static void CustomEntrySubject_76()
    {
        Enumerable.Sum(new long?[] { 10L, null, 30L });
    }

    // [77] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})
    public static void CustomEntrySubject_77()
    {
        Enumerable.Sum(new float?[] { 1.5f, null, 3.5f });
    }

    // [78] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})
    public static void CustomEntrySubject_78()
    {
        Enumerable.Sum(new double?[] { 1.5, null, 3.5 });
    }

    // [79] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_79()
    {
        Enumerable.Sum(new decimal?[] { 1.5m, null, 3.5m });
    }

    // ========================================================================
    // SUM — Selector non-nullable overloads (indices 80-84)
    // ========================================================================

    // [80] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})
    public static void CustomEntrySubject_80()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x);
    }

    // [81] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})
    public static void CustomEntrySubject_81()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (long)x);
    }

    // [82] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})
    public static void CustomEntrySubject_82()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (float)x);
    }

    // [83] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})
    public static void CustomEntrySubject_83()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (double)x);
    }

    // [84] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})
    public static void CustomEntrySubject_84()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => (decimal)x);
    }

    // ========================================================================
    // SUM — Selector nullable overloads (indices 85-89)
    // ========================================================================

    // [85] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})
    public static void CustomEntrySubject_85()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (int?)x : null);
    }

    // [86] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})
    public static void CustomEntrySubject_86()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (long?)x : null);
    }

    // [87] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})
    public static void CustomEntrySubject_87()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (float?)x : null);
    }

    // [88] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})
    public static void CustomEntrySubject_88()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (double?)x : null);
    }

    // [89] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})
    public static void CustomEntrySubject_89()
    {
        Enumerable.Sum(new int[] { 1, 2, 3 }, (int x) => x % 2 == 0 ? (decimal?)x : null);
    }
}
