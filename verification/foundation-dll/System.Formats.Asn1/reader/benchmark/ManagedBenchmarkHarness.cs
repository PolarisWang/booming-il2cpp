// Auto-generated managed benchmark harness
// Family: family/System.Formats.Asn1/reader
using System;
using System.Diagnostics;
using Chaos.Benchmark.reader;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::AsnReader::Void(System.Object)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_AsnReader_Void_System_Object),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Property:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Property_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method1:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method1_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method2_System_Void_P0_P1),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Validate:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_Validate_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Value:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Value_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Value_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Count:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Count_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Count_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_IsEmpty_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_IsEmpty_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Key:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Key_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Formats.Asn1\",");
Console.WriteLine("  \"familyId\": \"family/System.Formats.Asn1/reader\",");
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