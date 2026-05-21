// Auto-generated managed benchmark harness
// Family: sse-item, Assembly: System.Net.ServerSentEvents
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
            { // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_Data:T() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_Data:T()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventId:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventId:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventType:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventType:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_ReconnectionInterval:System.Nullable{System.TimeSpan}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_ReconnectionInterval:System.Nullable{System.TimeSpan}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String)",
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
