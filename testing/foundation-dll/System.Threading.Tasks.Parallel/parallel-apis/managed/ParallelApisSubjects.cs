// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/parallel-apis
// Assembly: System.Threading.Tasks.Parallel
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ParallelApisSubjects
{
    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32})
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((Parallel.For(0, 42, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64})
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((Parallel.For(0, 42, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32})
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64})
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { })).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_7()
    {
    }

    // [8] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_8()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [9] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_9()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [10] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_10()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [11] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_11()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [12] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_12()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    }

    // [13] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_13()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    }

    // [14] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_14()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    }

    // [15] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource})
    [Fact]
    public static void Subject_15()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [16] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})
    [Fact]
    public static void Subject_16()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [17] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_17()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [18] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_18()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [19] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    [Fact]
    public static void Subject_19()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [20] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    [Fact]
    public static void Subject_20()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [21] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_21()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [22] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_22()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [23] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_23()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    }

    // [24] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_24()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    }

    // [25] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource})
    [Fact]
    public static void Subject_25()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [26] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_26()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [27] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    [Fact]
    public static void Subject_27()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [28] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_28()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [29] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_29()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    }

    // [30] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})
    [Fact]
    public static void Subject_30()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [31] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    [Fact]
    public static void Subject_31()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [32] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    [Fact]
    public static void Subject_32()
    {
        try { var __val = ((Parallel.ForEach{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [33] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_33()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    }

    // [34] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    [Fact]
    public static void Subject_34()
    {
        // non-callable: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    }

    // [35] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_35()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [36] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_36()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [37] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_37()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [38] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_38()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [39] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_39()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [40] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    [Fact]
    public static void Subject_40()
    {
        try { var __val = ((Parallel.ForEachAsync{TSource}(default, default, default, default, default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [41] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action{})
    [Fact]
    public static void Subject_41()
    {
        try { Parallel.Invoke(() => { }); }
        catch { }
    }

    // [42] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{})
    public static void CustomEntrySubject_42()
    {
    }

}