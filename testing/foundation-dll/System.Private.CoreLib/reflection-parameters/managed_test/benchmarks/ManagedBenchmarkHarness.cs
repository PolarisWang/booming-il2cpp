// Auto-generated managed benchmark harness
// Family: reflection-parameters, Assembly: System.Private.CoreLib
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
            { // [0] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()",
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
