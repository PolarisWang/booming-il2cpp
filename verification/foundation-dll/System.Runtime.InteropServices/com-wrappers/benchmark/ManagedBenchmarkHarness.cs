// Auto-generated managed benchmark harness
// Family: family/System.Runtime.InteropServices/com-wrappers
using System;
using System.Diagnostics;
using Chaos.Benchmark.com_wrappers;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComWrappers::Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComWrappers_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComputeVtables:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComputeVtables_System_Void_P0_P1_P2),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1_P2_P3),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetIUnknownImpl_System_Void_P0_P1_P2),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateComInterfaceForObject_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0_P1_P2),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_P0_P1_P2),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_P0_P1_P2_P3),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForMarshalling_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForTrackerSupport_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ReleaseObjects:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ReleaseObjects_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetComInstance_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetObject_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers+ComInterfaceDispatch::GetInstance`1:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers+ComInterfaceDispatch_GetInstance_1_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.InteropServices\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.InteropServices/com-wrappers\",");
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