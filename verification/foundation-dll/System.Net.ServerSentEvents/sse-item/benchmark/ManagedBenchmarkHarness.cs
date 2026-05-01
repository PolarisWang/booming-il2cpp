// Auto-generated managed benchmark harness
// Family: family/System.Net.ServerSentEvents/sse-item
using System;
using System.Diagnostics;
using Chaos.Benchmark.sse_item;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::SseItem::Void(System.Object)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_SseItem_Void_System_Object),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Property:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Property_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method1:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method1_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method2_System_Void_P0_P1),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Validate:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Validate_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Value:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Value_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Value_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Count:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Count_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Count_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_IsEmpty_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_IsEmpty_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Key:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Key_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Net.ServerSentEvents\",");
Console.WriteLine("  \"familyId\": \"family/System.Net.ServerSentEvents/sse-item\",");
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