// Auto-generated managed benchmark harness
// Family: parallel-apis, Assembly: System.Threading.Tasks.Parallel
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int32,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For{TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{System.Int64,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.CancellationToken,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForAsync{T}:System.Threading.Tasks.Task(T,T,System.Threading.Tasks.ParallelOptions,System.Func{T,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Action{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.Partitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEach{TSource,TLocal}:System.Threading.Tasks.ParallelLoopResult(System.Collections.Concurrent.OrderablePartitioner{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TLocal},System.Func{TSource,System.Threading.Tasks.ParallelLoopState,System.Int64,TLocal,TLocal},System.Action{TLocal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.CancellationToken,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::ForEachAsync{TSource}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{TSource},System.Threading.Tasks.ParallelOptions,System.Func{TSource,System.Threading.CancellationToken,System.Threading.Tasks.ValueTask})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
