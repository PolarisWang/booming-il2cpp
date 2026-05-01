// Auto-generated managed benchmark harness
// Family: family/System.Text.Json/document-element
using System;
using System.Diagnostics;
using Chaos.Benchmark.document_element;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Text.Json/System.Text.Json.JsonDocument::JsonDocument::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_JsonDocument_Void_System_Object),
    ("System.Text.Json/System.Text.Json.JsonDocument::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.JsonDocument::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonDocument::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.JsonDocument::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::JsonElement::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_JsonElement_Void_System_Object),
    ("System.Text.Json/System.Text.Json.JsonElement::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.JsonElement::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.JsonElement::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonDocument::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.JsonDocument::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonDocument::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.JsonDocument::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonDocument::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.JsonDocument::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonDocument::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.JsonDocument::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonDocument_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.JsonElement::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.JsonElement::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.JsonElement::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.JsonElement::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.JsonElement::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_JsonElement_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Text.Json\",");
Console.WriteLine("  \"familyId\": \"family/System.Text.Json/document-element\",");
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