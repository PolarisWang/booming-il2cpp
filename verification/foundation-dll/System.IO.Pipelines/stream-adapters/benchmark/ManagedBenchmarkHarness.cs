// Auto-generated managed benchmark harness
// Family: family/System.IO.Pipelines/stream-adapters
using System;
using System.Diagnostics;
using Chaos.Benchmark.stream_adapters;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::StreamPipeExtensions::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_StreamPipeExtensions_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeExtensions_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Pipelines\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Pipelines/stream-adapters\",");
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