// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/generic/nullable-value
using System;
using System.Diagnostics;
using Chaos.Benchmark.generic_nullable_value;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_get_HasValue_System_Boolean),
    ("System.Private.CoreLib/System.Nullable`1::get_Value:T()", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_get_Value_T),
    ("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T),
    ("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_GetValueOrDefault_T_T),
    ("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_Equals_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_GetHashCode_System_Int32),
    ("System.Private.CoreLib/System.Nullable`1::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Nullable_1_ToString_System_String),
    ("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_ValueType_GetHashCode_System_Int32),
    ("System.Private.CoreLib/System.ValueType::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_ValueType_ToString_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/generic/nullable-value\",");
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