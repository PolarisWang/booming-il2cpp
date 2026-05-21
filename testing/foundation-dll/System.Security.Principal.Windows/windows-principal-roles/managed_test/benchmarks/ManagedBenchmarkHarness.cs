// Auto-generated managed benchmark harness
// Family: windows-principal-roles, Assembly: System.Security.Principal.Windows
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
            { // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Identity:System.Security.Principal.IIdentity() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Identity:System.Security.Principal.IIdentity()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.WindowsBuiltInRole) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.WindowsBuiltInRole)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.SecurityIdentifier) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.SecurityIdentifier)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::.ctor:System.Void(System.Security.Principal.WindowsIdentity) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::.ctor:System.Void(System.Security.Principal.WindowsIdentity)",
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
