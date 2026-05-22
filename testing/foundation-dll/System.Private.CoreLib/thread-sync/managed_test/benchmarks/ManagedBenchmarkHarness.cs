// Auto-generated managed benchmark harness
// Family: thread-sync, Assembly: System.Private.CoreLib
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

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_0(int i)
{
    System.Threading.Thread.Sleep(0);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [1] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.Threading.CancellationTokenSource::Cancel:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.CancellationTokenSource::Cancel:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.Threading.CancellationToken::get_IsCancellationRequested:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.CancellationToken::get_IsCancellationRequested:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Private.CoreLib/System.Threading.CancellationToken::Register:System.Threading.CancellationTokenRegistration(System.Action) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.CancellationToken::Register:System.Threading.CancellationTokenRegistration(System.Action)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.Threading.WaitCallback::.ctor:System.Void(System.Object,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Threading.WaitCallback::.ctor:System.Void(System.Object,System.IntPtr)",
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
