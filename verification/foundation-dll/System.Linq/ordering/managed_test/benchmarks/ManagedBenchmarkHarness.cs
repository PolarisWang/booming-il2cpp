// Auto-generated managed benchmark harness
// Family: ordering, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Order{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::OrderDescending{T}:System.Linq.IOrderedEnumerable{T}(System.Collections.Generic.IEnumerable{T},System.Collections.Generic.IComparer{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(TSource{}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Reverse{TSource}:System.Collections.Generic.IEnumerable{TSource}(TSource{})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ThenBy{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::ThenByDescending{TSource,TKey}:System.Linq.IOrderedEnumerable{TSource}(System.Linq.IOrderedEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
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
