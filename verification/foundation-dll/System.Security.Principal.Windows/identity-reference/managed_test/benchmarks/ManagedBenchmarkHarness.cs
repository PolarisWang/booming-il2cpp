// Auto-generated managed benchmark harness
// Family: identity-reference, Assembly: System.Security.Principal.Windows
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
            { // [0] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Equals:System.Boolean(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Equals:System.Boolean(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::GetHashCode:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::GetHashCode:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IsValidTargetType:System.Boolean(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IsValidTargetType:System.Boolean(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Equality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Equality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Inequality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Inequality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::ToString:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::ToString:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Translate:System.Security.Principal.IdentityReference(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Translate:System.Security.Principal.IdentityReference(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Add:System.Void(System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Add:System.Void(System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Clear:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Clear:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Contains:System.Boolean(System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Contains:System.Boolean(System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::CopyTo:System.Void(System.Security.Principal.IdentityReference{},System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::CopyTo:System.Void(System.Security.Principal.IdentityReference{},System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::GetEnumerator:System.Collections.Generic.IEnumerator{System.Security.Principal.IdentityReference}() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::GetEnumerator:System.Collections.Generic.IEnumerator{System.Security.Principal.IdentityReference}()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Remove:System.Boolean(System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Remove:System.Boolean(System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Int32,System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Int32,System.Security.Principal.IdentityReference)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Security.Principal.IdentityReference) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Security.Principal.IdentityReference)",
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
