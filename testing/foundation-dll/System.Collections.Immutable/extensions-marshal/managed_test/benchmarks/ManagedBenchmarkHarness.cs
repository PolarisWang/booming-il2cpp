// Auto-generated managed benchmark harness
// Family: extensions-marshal, Assembly: System.Collections.Immutable
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
            { // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)",
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
