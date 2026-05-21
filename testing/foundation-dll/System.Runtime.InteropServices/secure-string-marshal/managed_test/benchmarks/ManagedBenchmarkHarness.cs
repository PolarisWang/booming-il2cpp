// Auto-generated managed benchmark harness
// Family: secure-string-marshal, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)",
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
