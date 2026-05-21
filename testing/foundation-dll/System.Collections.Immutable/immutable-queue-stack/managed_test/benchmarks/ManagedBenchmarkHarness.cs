// Auto-generated managed benchmark harness
// Family: immutable-queue-stack, Assembly: System.Collections.Immutable
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
            { // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)",
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
