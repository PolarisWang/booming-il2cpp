// Auto-generated managed benchmark harness
// Family: family/System.Text.Json/serializer-options
using System;
using System.Diagnostics;
using Chaos.Benchmark.serializer_options;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::JsonNamingPolicy::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_JsonNamingPolicy_Void_System_Object),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::JsonSerializerOptions::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_JsonSerializerOptions_Void_System_Object),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Text.Json\",");
Console.WriteLine("  \"familyId\": \"family/System.Text.Json/serializer-options\",");
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