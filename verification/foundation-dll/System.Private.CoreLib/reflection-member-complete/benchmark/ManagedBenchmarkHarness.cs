// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/member/complete
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_member_complete;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32),
    ("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type),
    ("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle),
    ("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions),
    ("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle),
    ("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/member/complete\",");
Console.WriteLine("  \"warmupIterations\": " + kWarmupIterations + ",");
Console.WriteLine("  \"measureIterations\": " + kMeasureIterations + ",");
Console.WriteLine("  \"results\": [");

for (int i = 0; i < methodSubjects.Length; i++)
{
    var (subjectId, body) = methodSubjects[i];

    // Warmup
    for (int w = 0; w < kWarmupIterations; w++)
    {
        body();
    }

    // Measurement
    var sw = Stopwatch.StartNew();
    for (int m = 0; m < kMeasureIterations; m++)
    {
        body();
    }
    sw.Stop();

    double elapsedMs = sw.Elapsed.TotalMilliseconds;
    double opsPerSecond = kMeasureIterations / (elapsedMs / 1000.0);

    string comma = (i < methodSubjects.Length - 1) ? "," : "";
    Console.WriteLine("    {");
    Console.WriteLine("      \"methodIndex\": " + i + ",");
    Console.WriteLine("      \"methodSubjectId\": \"" + subjectId.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\",");
    Console.WriteLine("      \"elapsedMilliseconds\": " + elapsedMs.ToString("F6") + ",");
    Console.WriteLine("      \"opsPerSecond\": " + opsPerSecond.ToString("F6") + ",");
    Console.WriteLine("      \"iterations\": " + kMeasureIterations);
    Console.WriteLine("    }" + comma);
}

Console.WriteLine("  ]");
Console.WriteLine("}");