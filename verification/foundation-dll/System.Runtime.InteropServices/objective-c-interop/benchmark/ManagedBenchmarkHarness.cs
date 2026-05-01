// Auto-generated managed benchmark harness
// Family: family/System.Runtime.InteropServices/objective-c-interop
using System;
using System.Diagnostics;
using Chaos.Benchmark.objective_c_interop;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_CreateReferenceTrackingHandle_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_Initialize_System_Void_P0_P1_P2_P3),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendCallback_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendPendingException_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal+UnhandledExceptionPropagationHandler_ObjectiveCMarshal+UnhandledExceptionPropagationHandler_Void_System_Object_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::BeginInvoke:System.Void(P0, P1, P2, P3, P4)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal+UnhandledExceptionPropagationHandler_BeginInvoke_System_Void_P0_P1_P2_P3_P4),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::EndInvoke:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal+UnhandledExceptionPropagationHandler_EndInvoke_System_Void_P0_P1),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Invoke:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal+UnhandledExceptionPropagationHandler_Invoke_System_Void_P0_P1_P2),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCTrackedTypeAttribute::ObjectiveCTrackedTypeAttribute::Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCTrackedTypeAttribute_ObjectiveCTrackedTypeAttribute_Void)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.InteropServices\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.InteropServices/objective-c-interop\",");
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