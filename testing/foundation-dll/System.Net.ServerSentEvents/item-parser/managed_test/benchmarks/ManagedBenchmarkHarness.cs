// Auto-generated managed benchmark harness
// Family: item-parser, Assembly: System.Net.ServerSentEvents
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
            { // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::BeginInvoke:System.IAsyncResult(System.String,System.ReadOnlySpan{System.Byte},System.AsyncCallback,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::BeginInvoke:System.IAsyncResult(System.String,System.ReadOnlySpan{System.Byte},System.AsyncCallback,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::EndInvoke:T(System.IAsyncResult) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::EndInvoke:T(System.IAsyncResult)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::Invoke:T(System.String,System.ReadOnlySpan{System.Byte}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::Invoke:T(System.String,System.ReadOnlySpan{System.Byte})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::.ctor:System.Void(System.Object,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::.ctor:System.Void(System.Object,System.IntPtr)",
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
