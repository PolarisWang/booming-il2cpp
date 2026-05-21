// Auto-generated managed benchmark harness
// Family: data-errors, Assembly: System.ObjectModel
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
            { // [0] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::.ctor:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Collections.IEnumerable(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(System.EventHandler{System.ComponentModel.DataErrorsChangedEventArgs})",
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
