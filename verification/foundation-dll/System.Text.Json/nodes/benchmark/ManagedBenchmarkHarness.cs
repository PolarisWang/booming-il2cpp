// Auto-generated managed benchmark harness
// Family: family/System.Text.Json/nodes
using System;
using System.Diagnostics;
using Chaos.Benchmark.nodes;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::JsonArray::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_JsonArray_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::JsonNode::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_JsonNode_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::JsonObject::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_JsonObject_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::JsonValue::Void(System.Object)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_JsonValue_Void_System_Object),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Property:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Property_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::Method1:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_Method1_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_Method2_System_Void_P0_P1),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::Validate:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_Validate_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Key_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Value:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Value_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Value_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Count:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Count_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Count_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_get_IsEmpty_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_set_IsEmpty_System_Void_P0),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Key:System.Void()", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Key_System_Void),
    ("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Text.Json\",");
Console.WriteLine("  \"familyId\": \"family/System.Text.Json/nodes\",");
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