// Auto-generated managed benchmark harness
// Family: family/System.Security.Principal.Windows/nt-account
using System;
using System.Diagnostics;
using Chaos.Benchmark.nt_account;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::NTAccount::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_NTAccount_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Security.Principal.Windows\",");
Console.WriteLine("  \"familyId\": \"family/System.Security.Principal.Windows/nt-account\",");
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