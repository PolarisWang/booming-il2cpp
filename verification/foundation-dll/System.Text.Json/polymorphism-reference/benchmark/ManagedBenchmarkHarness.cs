// Auto-generated managed benchmark harness
// Family: family/System.Text.Json/polymorphism-reference
using System;
using System.Diagnostics;
using Chaos.Benchmark.polymorphism_reference;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::ReferenceHandler::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_ReferenceHandler_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ReferenceResolver::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_ReferenceResolver_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Text.Json\",");
Console.WriteLine("  \"familyId\": \"family/System.Text.Json/polymorphism-reference\",");
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