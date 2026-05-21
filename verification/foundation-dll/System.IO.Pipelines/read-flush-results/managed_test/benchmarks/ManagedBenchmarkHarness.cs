// Auto-generated managed benchmark harness
// Family: read-flush-results, Assembly: System.IO.Pipelines
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
            { // [0] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCanceled:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCanceled:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCompleted:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCompleted:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.IO.Pipelines/System.IO.Pipelines.FlushResult::.ctor:System.Void(System.Boolean,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.FlushResult::.ctor:System.Void(System.Boolean,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Buffer:System.Buffers.ReadOnlySequence{System.Byte}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Buffer:System.Buffers.ReadOnlySequence{System.Byte}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCanceled:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCanceled:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCompleted:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCompleted:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.IO.Pipelines/System.IO.Pipelines.ReadResult::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.IO.Pipelines/System.IO.Pipelines.ReadResult::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Boolean)",
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
