// Auto-generated managed benchmark harness
// Family: stream-adapters, Assembly: System.IO.Pipelines
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
            { // [0] System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)",
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
