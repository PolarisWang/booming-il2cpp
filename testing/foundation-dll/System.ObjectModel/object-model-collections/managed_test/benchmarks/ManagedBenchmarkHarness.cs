// Auto-generated managed benchmark harness
// Family: object-model-collections, Assembly: System.ObjectModel
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
            { // [0] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Boolean(TKey) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Boolean(TKey)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Collections.Generic.IEqualityComparer{TKey}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Collections.Generic.IEqualityComparer{TKey}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem(TKey) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem(TKey)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Boolean(TKey) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Boolean(TKey)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Boolean(TKey,TItem&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Boolean(TKey,TItem&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:TItem()",
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
