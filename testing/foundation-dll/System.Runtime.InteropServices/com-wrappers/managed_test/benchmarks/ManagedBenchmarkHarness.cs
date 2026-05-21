// Auto-generated managed benchmark harness
// Family: com-wrappers, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(System.IntPtr&,System.IntPtr&,System.IntPtr&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.IntPtr(System.Object,System.Runtime.InteropServices.CreateComInterfaceFlags)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Object(System.IntPtr,System.Runtime.InteropServices.CreateObjectFlags,System.Object,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(System.Runtime.InteropServices.ComWrappers)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(System.Runtime.InteropServices.ComWrappers)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Boolean(System.Object,System.IntPtr&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Boolean(System.IntPtr,System.Object&)",
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
