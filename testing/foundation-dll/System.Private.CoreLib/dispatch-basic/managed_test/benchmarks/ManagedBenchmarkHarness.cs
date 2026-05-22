// Auto-generated managed benchmark harness
// Family: dispatch-basic, Assembly: System.Private.CoreLib
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
            { // [0] DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()",
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
