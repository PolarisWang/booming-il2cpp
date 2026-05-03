// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/generics
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_generics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetGenericTypeDefinition_System_Type),
    ("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])", BenchmarkManagedBody.System_Private_CoreLib_System_Type_MakeGenericType_System_Type_System_Type),
    ("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetGenericArguments_System_Type),
    ("System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetGenericParameterConstraints_System_Type),
    ("System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_GenericParameterPosition_System_Int32),
    ("System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_ContainsGenericParameters_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsGenericTypeDefinition_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean),
    ("System.Private.CoreLib/System.Type::MakeArrayType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_MakeArrayType_System_Type),
    ("System.Private.CoreLib/System.Type::MakeByRefType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_MakeByRefType_System_Type),
    ("System.Private.CoreLib/System.Type::MakePointerType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_MakePointerType_System_Type)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/generics\",");
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