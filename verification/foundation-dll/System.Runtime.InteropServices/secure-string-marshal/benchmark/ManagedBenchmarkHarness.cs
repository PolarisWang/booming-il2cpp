// Auto-generated managed benchmark harness
// Family: family/System.Runtime.InteropServices/secure-string-marshal
using System;
using System.Diagnostics;
using Chaos.Benchmark.secure_string_marshal;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemAnsi_System_Void_P0),
    ("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemUnicode_System_Void_P0),
    ("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocAnsi_System_Void_P0),
    ("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocUnicode_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.InteropServices\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.InteropServices/secure-string-marshal\",");
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