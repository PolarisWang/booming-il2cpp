// Auto-generated managed benchmark harness
// Family: observable-collections, Assembly: System.ObjectModel
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
            { // [0] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(System.Collections.Specialized.NotifyCollectionChangedEventHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.IEnumerable{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.IEnumerable{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.List{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::.ctor:System.Void(System.Collections.Generic.List{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Collections.ObjectModel.ReadOnlyObservableCollection{T}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Collections.ObjectModel.ReadOnlyObservableCollection{T}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::.ctor:System.Void(System.Collections.ObjectModel.ObservableCollection{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::.ctor:System.Void(System.Collections.ObjectModel.ObservableCollection{T})",
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
