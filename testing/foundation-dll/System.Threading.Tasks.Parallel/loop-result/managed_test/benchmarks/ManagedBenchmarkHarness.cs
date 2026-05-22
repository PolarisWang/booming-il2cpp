// Auto-generated managed benchmark harness
// Family: loop-result, Assembly: System.Threading.Tasks.Parallel
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
            { // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsCompleted:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsCompleted:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_LowestBreakIteration:System.Nullable{System.Int64}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_LowestBreakIteration:System.Nullable{System.Int64}()",
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
