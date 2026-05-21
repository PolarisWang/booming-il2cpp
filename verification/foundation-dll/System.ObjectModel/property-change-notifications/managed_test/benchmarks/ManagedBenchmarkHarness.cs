// Auto-generated managed benchmark harness
// Family: property-change-notifications, Assembly: System.ObjectModel
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
            { // [0] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(System.ComponentModel.PropertyChangedEventHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(System.ComponentModel.PropertyChangingEventHandler)",
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
