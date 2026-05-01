// Auto-generated managed benchmark harness
// Family: family/System.Net.ServerSentEvents/formatter
using System;
using System.Diagnostics;
using Chaos.Benchmark.formatter;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::SseFormatter::Void(System.Object)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_SseFormatter_Void_System_Object),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Property:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Property_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Method1:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Method1_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Method2_System_Void_P0_P1),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Validate:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Validate_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Value:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Value_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Value_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Count:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Count_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Count_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_IsEmpty_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_IsEmpty_System_Void_P0),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Key:System.Void()", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Key_System_Void),
    ("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Net.ServerSentEvents\",");
Console.WriteLine("  \"familyId\": \"family/System.Net.ServerSentEvents/formatter\",");
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