// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/attributes/custom-metadata
using System;
using System.Diagnostics;
using Chaos.Benchmark.attributes_custom_metadata;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type),
    ("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type),
    ("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type),
    ("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type),
    ("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo),
    ("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type),
    ("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type),
    ("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Attribute_get_TypeId_System_Object)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/attributes/custom-metadata\",");
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