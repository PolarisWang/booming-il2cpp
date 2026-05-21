// Auto-generated managed benchmark harness
// Family: pinvoke-dllimport, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::.ctor:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::.ctor:System.Void(System.String)",
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
