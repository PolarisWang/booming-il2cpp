// Auto-generated managed benchmark harness
// Family: aggregation, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::Aggregate{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TSource,TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Aggregate{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TSource,TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate}:TAccumulate(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate}:TAccumulate(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Func{TAccumulate,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Func{TAccumulate,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,TAccumulate},System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,TAccumulate},System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::CountBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,System.Int32}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::CountBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,System.Int32}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Int32&)",
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
