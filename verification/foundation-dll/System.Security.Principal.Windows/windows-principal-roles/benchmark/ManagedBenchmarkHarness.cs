// Auto-generated managed benchmark harness
// Family: family/System.Security.Principal.Windows/windows-principal-roles
using System;
using System.Diagnostics;
using Chaos.Benchmark.windows_principal_roles;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::WindowsAccountType::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_WindowsAccountType_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::WindowsBuiltInRole::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_WindowsBuiltInRole_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::WindowsPrincipal::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_WindowsPrincipal_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Key_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Key_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Security.Principal.Windows\",");
Console.WriteLine("  \"familyId\": \"family/System.Security.Principal.Windows/windows-principal-roles\",");
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