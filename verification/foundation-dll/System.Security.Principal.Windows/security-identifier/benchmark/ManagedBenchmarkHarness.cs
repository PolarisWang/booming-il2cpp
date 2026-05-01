// Auto-generated managed benchmark harness
// Family: family/System.Security.Principal.Windows/security-identifier
using System;
using System.Diagnostics;
using Chaos.Benchmark.security_identifier;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::SecurityIdentifier::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_SecurityIdentifier_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Security.Principal.Windows\",");
Console.WriteLine("  \"familyId\": \"family/System.Security.Principal.Windows/security-identifier\",");
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