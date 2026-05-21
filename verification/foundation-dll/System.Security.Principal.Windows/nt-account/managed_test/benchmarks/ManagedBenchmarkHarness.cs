// Auto-generated managed benchmark harness
// Family: nt-account, Assembly: System.Security.Principal.Windows
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
            { // [0] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Equals:System.Boolean(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::Equals:System.Boolean(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Security.Principal.Windows/System.Security.Principal.NTAccount::GetHashCode:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::GetHashCode:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Security.Principal.Windows/System.Security.Principal.NTAccount::IsValidTargetType:System.Boolean(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::IsValidTargetType:System.Boolean(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Equality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Equality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Inequality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Inequality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Security.Principal.Windows/System.Security.Principal.NTAccount::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Translate:System.Security.Principal.IdentityReference(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::Translate:System.Security.Principal.IdentityReference(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String)",
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
