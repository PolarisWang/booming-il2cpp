// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/parallel/apis
// Assembly: System.Threading.Tasks.Parallel
// Variant: subjects

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ParallelApisSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32})
    public static void Subject_0()
    {
        if (((Parallel.For(0, 42, (int x) => { })).GetHashCode()) != 410570569) _exitCode = 1;
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64})
    public static void Subject_1()
    {
        if (((Parallel.For(0, 42, (int x) => { })).GetHashCode()) != 410570569) _exitCode = 1;
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32})
    public static void Subject_2()
    {
        try { Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { }); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64})
    public static void Subject_3()
    {
        try { Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { }); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})
    public static void Subject_4()
    {
        try { Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { }); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})
    public static void Subject_5()
    {
        try { Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { }); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})
    public static void Subject_6()
    {
        // TODO: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState}) could not be auto-generated
    }

    // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})
    public static void Subject_7()
    {
        // TODO: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState}) could not be auto-generated
    }

    // [8] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_8()
    {
    }

    // [9] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_9()
    {
    }

    // [10] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_11()
    {
    }

    // [12] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_12()
    {
    }

    // [13] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_13()
    {
    }

    // [14] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_14()
    {
    }

    // [15] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource})
    public static void CustomEntrySubject_15()
    {
    }

    // [16] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})
    public static void CustomEntrySubject_16()
    {
    }

    // [17] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_17()
    {
    }

    // [18] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_18()
    {
    }

    // [19] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    public static void CustomEntrySubject_19()
    {
    }

    // [20] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    public static void CustomEntrySubject_20()
    {
    }

    // [21] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_21()
    {
    }

    // [22] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_22()
    {
    }

    // [23] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_23()
    {
    }

    // [24] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_24()
    {
    }

    // [25] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource})
    public static void CustomEntrySubject_25()
    {
    }

    // [26] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_26()
    {
    }

    // [27] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    public static void CustomEntrySubject_27()
    {
    }

    // [28] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_28()
    {
    }

    // [29] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_29()
    {
    }

    // [30] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})
    public static void CustomEntrySubject_30()
    {
    }

    // [31] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_31()
    {
    }

    // [32] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})
    public static void CustomEntrySubject_32()
    {
    }

    // [33] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_33()
    {
    }

    // [34] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})
    public static void CustomEntrySubject_34()
    {
    }

    // [35] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_35()
    {
    }

    // [36] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_36()
    {
    }

    // [37] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_37()
    {
    }

    // [38] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_38()
    {
    }

    // [39] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_39()
    {
    }

    // [40] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})
    public static void CustomEntrySubject_40()
    {
    }

    // [41] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action{})
    public static void Subject_41()
    {
        Parallel.Invoke(() => { });
    }

    // [42] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{})
    public static void Subject_42()
    {
        // TODO: System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{}) could not be auto-generated
    }

}