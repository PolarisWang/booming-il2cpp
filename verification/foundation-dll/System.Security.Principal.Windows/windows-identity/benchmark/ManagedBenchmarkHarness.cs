// Auto-generated managed benchmark harness
// Family: family/System.Security.Principal.Windows/windows-identity
using System;
using System.Diagnostics;
using Chaos.Benchmark.windows_identity;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::WindowsIdentity::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_WindowsIdentity_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Security.Principal.Windows\",");
Console.WriteLine("  \"familyId\": \"family/System.Security.Principal.Windows/windows-identity\",");
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