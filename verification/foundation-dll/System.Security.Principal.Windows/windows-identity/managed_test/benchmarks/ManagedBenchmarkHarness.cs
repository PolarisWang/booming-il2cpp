// Auto-generated managed benchmark harness
// Family: windows-identity, Assembly: System.Security.Principal.Windows
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
            { // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Clone:System.Security.Claims.ClaimsIdentity() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Clone:System.Security.Claims.ClaimsIdentity()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AccessToken:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AccessToken:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AuthenticationType:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AuthenticationType:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Groups:System.Security.Principal.IdentityReferenceCollection() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Groups:System.Security.Principal.IdentityReferenceCollection()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_ImpersonationLevel:System.Security.Principal.TokenImpersonationLevel() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_ImpersonationLevel:System.Security.Principal.TokenImpersonationLevel()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAnonymous:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAnonymous:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAuthenticated:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAuthenticated:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsGuest:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsGuest:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsSystem:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsSystem:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Name:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Name:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Owner:System.Security.Principal.SecurityIdentifier() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Owner:System.Security.Principal.SecurityIdentifier()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Token:System.IntPtr() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Token:System.IntPtr()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_User:System.Security.Principal.SecurityIdentifier() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_User:System.Security.Principal.SecurityIdentifier()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetAnonymous:System.Security.Principal.WindowsIdentity() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetAnonymous:System.Security.Principal.WindowsIdentity()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Security.Principal.TokenAccessLevels) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Security.Principal.TokenAccessLevels)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated:System.Void(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Action) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated:System.Void(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Action)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated{T}:T(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{T}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated{T}:T(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{T})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync:System.Threading.Tasks.Task(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync:System.Threading.Tasks.Task(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync{T}:System.Threading.Tasks.Task{T}(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task{T}}) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync{T}:System.Threading.Tasks.Task{T}(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task{T}})",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)",
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
