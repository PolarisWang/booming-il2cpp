// Auto-generated managed benchmark harness
// Family: source-generation-context, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)",
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
