// Auto-generated managed benchmark harness
// Family: family/System.Security.Principal.Windows/identity-reference
using System;
using System.Diagnostics;
using Chaos.Benchmark.identity_reference;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IdentityReference::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_IdentityReference_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::IdentityReferenceCollection::Void(System.Object)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_IdentityReferenceCollection_Void_System_Object),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Property:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Property_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method1:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method1_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method2_System_Void_P0_P1),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Validate:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Validate_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Key_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Value:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Value_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Value_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Count_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Count_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_IsEmpty_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_IsEmpty_System_Void_P0),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Key:System.Void()", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Key_System_Void),
    ("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Security.Principal.Windows\",");
Console.WriteLine("  \"familyId\": \"family/System.Security.Principal.Windows/identity-reference\",");
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