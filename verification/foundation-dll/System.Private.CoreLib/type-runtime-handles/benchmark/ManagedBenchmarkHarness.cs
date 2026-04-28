// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/type/runtime-handles
using System;
using System.Diagnostics;
using Chaos.Benchmark.type_runtime_handles;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String),
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean),
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean),
    ("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle),
    ("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsArray_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle),
    ("System.Private.CoreLib/System.Type::get_FullName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_FullName_System_String),
    ("System.Private.CoreLib/System.Type::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_Name_System_String),
    ("System.Private.CoreLib/System.Type::get_BaseType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_BaseType_System_Type),
    ("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/type/runtime-handles\",");
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