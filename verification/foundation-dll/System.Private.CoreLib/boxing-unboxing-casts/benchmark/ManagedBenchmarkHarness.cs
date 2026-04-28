// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/boxing/unboxing-casts
using System;
using System.Diagnostics;
using Chaos.Benchmark.boxing_unboxing_casts;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type),
    ("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider),
    ("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type),
    ("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type),
    ("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type),
    ("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Object_MemberwiseClone_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object),
    ("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32),
    ("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/boxing/unboxing-casts\",");
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