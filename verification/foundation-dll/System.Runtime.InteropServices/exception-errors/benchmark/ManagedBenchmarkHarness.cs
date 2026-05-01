// Auto-generated managed benchmark harness
// Family: family/System.Runtime.InteropServices/exception-errors
using System;
using System.Diagnostics;
using Chaos.Benchmark.exception_errors;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void),
    ("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object),
    ("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_ToString_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_CanResume_System_Void)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.InteropServices\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.InteropServices/exception-errors\",");
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