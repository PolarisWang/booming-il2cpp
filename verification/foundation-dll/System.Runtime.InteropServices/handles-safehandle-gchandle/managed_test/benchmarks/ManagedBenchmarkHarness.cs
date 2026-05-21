// Auto-generated managed benchmark harness
// Family: handles-safehandle-gchandle, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::AddrOfPinnedObject:System.IntPtr() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::AddrOfPinnedObject:System.IntPtr()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object,System.Runtime.InteropServices.GCHandleType) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object,System.Runtime.InteropServices.GCHandleType)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Runtime.InteropServices.GCHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Runtime.InteropServices.GCHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Free:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Free:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_IsAllocated:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_IsAllocated:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_Target:System.Object() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_Target:System.Object()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::GetHashCode:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::GetHashCode:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Equality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Equality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Inequality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Inequality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle)",
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
