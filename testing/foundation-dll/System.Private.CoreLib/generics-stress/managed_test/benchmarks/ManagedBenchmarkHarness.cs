// Auto-generated managed benchmark harness
// Family: generics-stress, Assembly: System.Private.CoreLib
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

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_1(int i)
{
    System.Array.BinarySearch(new byte[]{10,20,30,40}, (object)(byte)30);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_2(int i)
{
    System.Array.IndexOf(new byte[]{5,3,5,3}, (object)(byte)3);
    _g++;  // volatile side-effect prevents DCE
    return false;
}

    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Int32,System.Int32,System.Collections.Generic.IComparer<System.Int32>) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Int32,System.Int32,System.Collections.Generic.IComparer<System.Int32>)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [2] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
                bool threw = false;
                // Warmup: JIT compile before measurement
                for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                }
                // 3 rounds, take minimum to reduce GC/scheduling noise
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
