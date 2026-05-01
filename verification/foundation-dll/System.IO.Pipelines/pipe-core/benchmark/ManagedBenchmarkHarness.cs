// Auto-generated managed benchmark harness
// Family: family/System.IO.Pipelines/pipe-core
using System;
using System.Diagnostics;
using Chaos.Benchmark.pipe_core;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::IDuplexPipe::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_IDuplexPipe_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::Pipe::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_Pipe_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_IDuplexPipe_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.Pipe::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_Pipe_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Pipelines\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Pipelines/pipe-core\",");
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