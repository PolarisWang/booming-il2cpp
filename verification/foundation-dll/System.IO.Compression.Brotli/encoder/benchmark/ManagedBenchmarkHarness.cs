// Auto-generated managed benchmark harness
// Family: family/System.IO.Compression.Brotli/encoder
using System;
using System.Diagnostics;
using Chaos.Benchmark.encoder;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::BrotliEncoder::Void(System.Object)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_BrotliEncoder_Void_System_Object),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Property_System_Void),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method1_System_Void_P0),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Method2_System_Void_P0_P1),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_Validate_System_Void_P0),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Value_System_Void),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Value_System_Void_P0),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Count_System_Void),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Count_System_Void_P0),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_IsEmpty_System_Void),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_IsEmpty_System_Void_P0),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_get_Key_System_Void),
    ("System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoder_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Compression.Brotli\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Compression.Brotli/encoder\",");
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