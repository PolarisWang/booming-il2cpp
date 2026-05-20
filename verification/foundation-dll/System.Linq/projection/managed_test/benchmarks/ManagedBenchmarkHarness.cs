// Auto-generated managed benchmark harness
// Family: projection, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::Select{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Select{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::Select{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Select{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::SelectMany{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Collections.Generic.IEnumerable{TResult}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SelectMany{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Collections.Generic.IEnumerable{TResult}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::SelectMany{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Collections.Generic.IEnumerable{TResult}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SelectMany{TSource,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Collections.Generic.IEnumerable{TResult}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::SelectMany{TSource,TCollection,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Collections.Generic.IEnumerable{TCollection}},System.Func{TSource,TCollection,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SelectMany{TSource,TCollection,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Collections.Generic.IEnumerable{TCollection}},System.Func{TSource,TCollection,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::SelectMany{TSource,TCollection,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Collections.Generic.IEnumerable{TCollection}},System.Func{TSource,TCollection,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::SelectMany{TSource,TCollection,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Collections.Generic.IEnumerable{TCollection}},System.Func{TSource,TCollection,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond},System.Func{TFirst,TSecond,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond},System.Func{TFirst,TSecond,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond}:System.Collections.Generic.IEnumerable{System.ValueTuple{TFirst,TSecond}}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond}:System.Collections.Generic.IEnumerable{System.ValueTuple{TFirst,TSecond}}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond,TThird}:System.Collections.Generic.IEnumerable{System.ValueTuple{TFirst,TSecond,TThird}}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond},System.Collections.Generic.IEnumerable{TThird}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Zip{TFirst,TSecond,TThird}:System.Collections.Generic.IEnumerable{System.ValueTuple{TFirst,TSecond,TThird}}(System.Collections.Generic.IEnumerable{TFirst},System.Collections.Generic.IEnumerable{TSecond},System.Collections.Generic.IEnumerable{TThird})",
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
