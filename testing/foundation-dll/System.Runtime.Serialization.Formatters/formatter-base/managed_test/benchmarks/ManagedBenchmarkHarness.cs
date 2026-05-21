// Auto-generated managed benchmark harness
// Family: formatter-base, Assembly: System.Runtime.Serialization.Formatters
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
            { // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Object(System.IO.Stream) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Object(System.IO.Stream)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Runtime.Serialization.SerializationBinder() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Runtime.Serialization.SerializationBinder()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Runtime.Serialization.StreamingContext() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Runtime.Serialization.StreamingContext()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(System.IO.Stream,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(System.IO.Stream,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Object(System.IO.Stream) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Object(System.IO.Stream)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Runtime.Serialization.StreamingContext() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Runtime.Serialization.StreamingContext()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(System.IO.Stream,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(System.IO.Stream,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)",
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
