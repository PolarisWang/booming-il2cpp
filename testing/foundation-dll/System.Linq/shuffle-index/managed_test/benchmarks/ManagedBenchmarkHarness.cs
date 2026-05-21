// Auto-generated managed benchmark harness
// Family: shuffle-index, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::ElementAt{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ElementAt{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::ElementAt{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Index) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ElementAt{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Index)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::ElementAtOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ElementAtOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::ElementAtOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Index) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ElementAtOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Index)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::First{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::First{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::First{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::First{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::FirstOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Linq/System.Linq.Enumerable::Index{TSource}:System.Collections.Generic.IEnumerable{System.ValueTuple{System.Int32,TSource}}(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Index{TSource}:System.Collections.Generic.IEnumerable{System.ValueTuple{System.Int32,TSource}}(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Linq/System.Linq.Enumerable::Last{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Last{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Linq/System.Linq.Enumerable::Last{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Last{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LastOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Linq/System.Linq.Enumerable::Shuffle{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Shuffle{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Linq/System.Linq.Enumerable::Single{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Single{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Linq/System.Linq.Enumerable::Single{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Single{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},TSource)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SingleOrDefault{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean},TSource)",
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
