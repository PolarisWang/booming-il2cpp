// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/module
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_module;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_get_FullyQualifiedName_System_String),
    ("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_get_Name_System_String),
    ("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_get_Assembly_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_GetType_System_Type_System_String),
    ("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_GetTypes_System_Type),
    ("System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Module_GetCustomAttributes_System_Object_System_Type)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/module\",");
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