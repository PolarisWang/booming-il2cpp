// Auto-generated managed benchmark harness
// Family: codegen-edge-cases, Assembly: System.Private.CoreLib
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
            { // [0] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_0:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_0:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_1:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_1:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_2:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_2:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_3:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_3:System.Int32(System.Int32)",
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
