// Auto-generated managed benchmark harness
// Family: family/System.IO.Pipelines/reader-writer
using System;
using System.Diagnostics;
using Chaos.Benchmark.reader_writer;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::PipeReader::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_PipeReader_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::PipeWriter::Void(System.Object)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_PipeWriter_Void_System_Object),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Property_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method1_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method2_System_Void_P0_P1),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Validate_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Key_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Value_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Value_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Count_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Count_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_IsEmpty_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_IsEmpty_System_Void_P0),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Key_System_Void),
    ("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Pipelines\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Pipelines/reader-writer\",");
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