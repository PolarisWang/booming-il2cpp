// Auto-generated managed benchmark harness
// Family: family/System.IO.Compression.ZipFile/zip-file-extensions
using System;
using System.Diagnostics;
using Chaos.Benchmark.zip_file_extensions;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::ZipFileExtensions::Void(System.Object)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_ZipFileExtensions_Void_System_Object),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Property:System.Void()", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Property_System_Void),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method1:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method1_System_Void_P0),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Method2_System_Void_P0_P1),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::Validate:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_Validate_System_Void_P0),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Value:System.Void()", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Value_System_Void),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Value_System_Void_P0),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Count:System.Void()", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Count_System_Void),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Count_System_Void_P0),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_IsEmpty_System_Void),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_IsEmpty_System_Void_P0),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::get_Key:System.Void()", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_get_Key_System_Void),
    ("System.IO.Compression.ZipFile/System.IO.Compression.ZipFileExtensions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_IO_Compression_ZipFile_System_IO_Compression_ZipFileExtensions_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.IO.Compression.ZipFile\",");
Console.WriteLine("  \"familyId\": \"family/System.IO.Compression.ZipFile/zip-file-extensions\",");
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