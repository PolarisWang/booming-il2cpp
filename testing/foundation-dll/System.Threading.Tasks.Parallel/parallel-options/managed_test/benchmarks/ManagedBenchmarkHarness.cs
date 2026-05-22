// Auto-generated managed benchmark harness
// Family: parallel-options, Assembly: System.Threading.Tasks.Parallel
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
            { // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_CancellationToken:System.Threading.CancellationToken() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_CancellationToken:System.Threading.CancellationToken()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_MaxDegreeOfParallelism:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_MaxDegreeOfParallelism:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_TaskScheduler:System.Threading.Tasks.TaskScheduler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_TaskScheduler:System.Threading.Tasks.TaskScheduler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_CancellationToken:System.Void(System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_CancellationToken:System.Void(System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_MaxDegreeOfParallelism:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_MaxDegreeOfParallelism:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_TaskScheduler:System.Void(System.Threading.Tasks.TaskScheduler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_TaskScheduler:System.Void(System.Threading.Tasks.TaskScheduler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::.ctor:System.Void()",
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
