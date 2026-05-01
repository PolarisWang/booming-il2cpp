// Auto-generated managed benchmark harness
// Family: family/System.IO.Pipelines/options-scheduler
using System;
using System.Diagnostics;
using Chaos.Benchmark.options_scheduler;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::PipeOptions::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_PipeOptions_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::PipeScheduler::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_PipeScheduler_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::StreamPipeReaderOptions::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_StreamPipeReaderOptions_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::StreamPipeWriterOptions::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_StreamPipeWriterOptions_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeOptions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeOptions_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeScheduler_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeReaderOptions_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_StreamPipeWriterOptions_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Pipelines\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Pipelines/options-scheduler\",");
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