// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/member-basics
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_member_basics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes),
    ("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type),
    ("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodInfo_GetParameters_System_Reflection_ParameterInfo),
    ("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type),
    ("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_GetValue_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/member-basics\",");
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