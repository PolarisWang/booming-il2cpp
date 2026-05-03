// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/parameters
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_parameters;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_Name_System_String),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_ParameterType_System_Type),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_DefaultValue_System_Object),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_RawDefaultValue_System_Object),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_HasDefaultValue_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_Attributes_System_Reflection_ParameterAttributes),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_get_Position_System_Int32),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_GetRequiredCustomModifiers_System_Type),
    ("System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ParameterInfo_GetOptionalCustomModifiers_System_Type),
    ("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_GetParameters_System_Reflection_ParameterInfo)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/parameters\",");
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