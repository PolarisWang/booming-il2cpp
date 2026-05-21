// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/parallel-apis
// Assembly: System.Threading.Tasks.Parallel
// Variant: benchmark

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ParallelApisNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32})

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64})

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32})

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64})

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})

    // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})

    // [8] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [9] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [10] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [11] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [12] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [13] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [14] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [15] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource})

    // [16] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})

    // [17] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})

    // [18] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})

    // [19] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})

    // [20] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})

    // [21] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [22] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [23] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})

    // [24] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})

    // [25] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource})

    // [26] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})

    // [27] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})

    // [28] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [29] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})

    // [30] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})

    // [31] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})

    // [32] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})

    // [33] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})

    // [34] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})

    // [35] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [36] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [37] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [38] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [39] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [40] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})

    // [41] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action{})

    // [42] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{})

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: CustomEntryMethod9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: CustomEntryMethod12(); break;
                case 13: CustomEntryMethod13(); break;
                case 14: CustomEntryMethod14(); break;
                case 15: CustomEntryMethod15(); break;
                case 16: CustomEntryMethod16(); break;
                case 17: CustomEntryMethod17(); break;
                case 18: CustomEntryMethod18(); break;
                case 19: CustomEntryMethod19(); break;
                case 20: CustomEntryMethod20(); break;
                case 21: CustomEntryMethod21(); break;
                case 22: CustomEntryMethod22(); break;
                case 23: CustomEntryMethod23(); break;
                case 24: CustomEntryMethod24(); break;
                case 25: CustomEntryMethod25(); break;
                case 26: CustomEntryMethod26(); break;
                case 27: CustomEntryMethod27(); break;
                case 28: CustomEntryMethod28(); break;
                case 29: CustomEntryMethod29(); break;
                case 30: CustomEntryMethod30(); break;
                case 31: CustomEntryMethod31(); break;
                case 32: CustomEntryMethod32(); break;
                case 33: CustomEntryMethod33(); break;
                case 34: CustomEntryMethod34(); break;
                case 35: CustomEntryMethod35(); break;
                case 36: CustomEntryMethod36(); break;
                case 37: CustomEntryMethod37(); break;
                case 38: CustomEntryMethod38(); break;
                case 39: CustomEntryMethod39(); break;
                case 40: CustomEntryMethod40(); break;
                case 41: CustomEntryMethod41(); break;
                case 42: CustomEntryMethod42(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}