// Auto-generated managed benchmark harness
// Family: reflection-generics, Assembly: System.Private.CoreLib
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
            { // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[]) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::MakeArrayType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::MakeByRefType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::MakePointerType:System.Type()",
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
