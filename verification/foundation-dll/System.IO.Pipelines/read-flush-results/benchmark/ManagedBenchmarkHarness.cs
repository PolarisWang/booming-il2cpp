// Auto-generated managed benchmark harness
// Family: family/System.IO.Pipelines/read-flush-results
using System;
using System.Diagnostics;
using Chaos.Benchmark.read_flush_results;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::FlushResult::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_FlushResult_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::ReadResult::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_ReadResult_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Pipelines\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Pipelines/read-flush-results\",");
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