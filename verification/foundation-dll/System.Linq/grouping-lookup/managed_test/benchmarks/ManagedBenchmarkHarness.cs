// Auto-generated managed benchmark harness
// Family: grouping-lookup, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TSource}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TSource}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TSource}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TSource}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TElement}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TElement}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TElement}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement}:System.Collections.Generic.IEnumerable{System.Linq.IGrouping{TKey,TElement}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,System.Collections.Generic.IEnumerable{TSource},TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,System.Collections.Generic.IEnumerable{TSource},TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,System.Collections.Generic.IEnumerable{TSource},TResult},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,System.Collections.Generic.IEnumerable{TSource},TResult},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Func{TKey,System.Collections.Generic.IEnumerable{TElement},TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Func{TKey,System.Collections.Generic.IEnumerable{TElement},TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Func{TKey,System.Collections.Generic.IEnumerable{TElement},TResult},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::GroupBy{TSource,TKey,TElement,TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TSource,TElement},System.Func{TKey,System.Collections.Generic.IEnumerable{TElement},TResult},System.Collections.Generic.IEqualityComparer{TKey})",
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
