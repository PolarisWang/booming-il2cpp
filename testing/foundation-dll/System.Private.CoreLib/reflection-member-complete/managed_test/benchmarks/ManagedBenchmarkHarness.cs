// Auto-generated managed benchmark harness
// Family: reflection-member-complete, Assembly: System.Private.CoreLib
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
            { // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()",
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
