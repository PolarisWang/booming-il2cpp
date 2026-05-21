// Auto-generated managed benchmark harness
// Family: formatter, Assembly: System.Net.ServerSentEvents
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
            { // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{System.String}},System.IO.Stream,System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{System.String}},System.IO.Stream,System.Threading.CancellationToken)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync{T}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}},System.IO.Stream,System.Action{System.Net.ServerSentEvents.SseItem{T},System.Buffers.IBufferWriter{System.Byte}},System.Threading.CancellationToken) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync{T}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}},System.IO.Stream,System.Action{System.Net.ServerSentEvents.SseItem{T},System.Buffers.IBufferWriter{System.Byte}},System.Threading.CancellationToken)",
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
