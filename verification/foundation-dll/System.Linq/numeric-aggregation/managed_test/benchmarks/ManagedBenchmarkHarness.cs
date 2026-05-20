// Auto-generated managed benchmark harness
// Family: numeric-aggregation, Assembly: System.Linq
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
            { // [0] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable{System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable{System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable{System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Average{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Linq/System.Linq.Enumerable::Max:System.Int32(System.Collections.Generic.IEnumerable{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Linq/System.Linq.Enumerable::Max:System.Int64(System.Collections.Generic.IEnumerable{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Linq/System.Linq.Enumerable::Max:System.Double(System.Collections.Generic.IEnumerable{System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Double(System.Collections.Generic.IEnumerable{System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Linq/System.Linq.Enumerable::Max:System.Single(System.Collections.Generic.IEnumerable{System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Single(System.Collections.Generic.IEnumerable{System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Linq/System.Linq.Enumerable::Max:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Linq/System.Linq.Enumerable::Max{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Max{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::MaxBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Linq/System.Linq.Enumerable::Min:System.Int32(System.Collections.Generic.IEnumerable{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Linq/System.Linq.Enumerable::Min:System.Int64(System.Collections.Generic.IEnumerable{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Linq/System.Linq.Enumerable::Min:System.Single(System.Collections.Generic.IEnumerable{System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Single(System.Collections.Generic.IEnumerable{System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Linq/System.Linq.Enumerable::Min:System.Double(System.Collections.Generic.IEnumerable{System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Double(System.Collections.Generic.IEnumerable{System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Linq/System.Linq.Enumerable::Min:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Collections.Generic.IComparer{TSource})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Linq/System.Linq.Enumerable::Min{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Min{TSource,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TResult})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [69] System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 69,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::MinBy{TSource,TKey}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IComparer{TKey})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [70] System.Linq/System.Linq.Enumerable::Sum:System.Int32(System.Collections.Generic.IEnumerable{System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 70,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Int32(System.Collections.Generic.IEnumerable{System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [71] System.Linq/System.Linq.Enumerable::Sum:System.Int64(System.Collections.Generic.IEnumerable{System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 71,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Int64(System.Collections.Generic.IEnumerable{System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [72] System.Linq/System.Linq.Enumerable::Sum:System.Single(System.Collections.Generic.IEnumerable{System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 72,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Single(System.Collections.Generic.IEnumerable{System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [73] System.Linq/System.Linq.Enumerable::Sum:System.Double(System.Collections.Generic.IEnumerable{System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 73,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Double(System.Collections.Generic.IEnumerable{System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [74] System.Linq/System.Linq.Enumerable::Sum:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 74,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Decimal(System.Collections.Generic.IEnumerable{System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [75] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 75,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [76] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 76,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [77] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 77,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [78] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 78,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [79] System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 79,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{System.Nullable{System.Decimal}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [80] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 80,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [81] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 81,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int64})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [82] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 82,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Single(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Single})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [83] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 83,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Double(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Double})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [84] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 84,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Decimal(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Decimal})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [85] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 85,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int32}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int32}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [86] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 86,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Int64}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Int64}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [87] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 87,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Single}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Single}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [88] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 88,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Double}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Double}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [89] System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 89,
                    MethodSubjectId = "System.Linq/System.Linq.Enumerable::Sum{TSource}:System.Nullable{System.Decimal}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Nullable{System.Decimal}})",
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
