// Auto-generated managed benchmark harness
// Family: objective-c-interop, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Runtime.InteropServices.GCHandle(System.Object,System.Span{System.IntPtr}&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Runtime.InteropServices.GCHandle(System.Object,System.Span{System.IntPtr}&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+MessageSendFunction,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+MessageSendFunction,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(System.Exception)",
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
