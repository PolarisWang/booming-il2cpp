// Auto-generated managed benchmark harness
// Family: polymorphism-reference, Assembly: System.Text.Json
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
            { // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IgnoreCycles:System.Text.Json.Serialization.ReferenceHandler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IgnoreCycles:System.Text.Json.Serialization.ReferenceHandler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Preserve:System.Text.Json.Serialization.ReferenceHandler() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Preserve:System.Text.Json.Serialization.ReferenceHandler()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)",
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
