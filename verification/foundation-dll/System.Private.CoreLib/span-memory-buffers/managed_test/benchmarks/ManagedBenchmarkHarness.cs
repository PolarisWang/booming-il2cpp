// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/span/memory-buffers
using System;
using System.Diagnostics;
using Chaos.Benchmark.span_memory_buffers;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_get_Item_T_Ref_System_Int32),
    ("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32),
    ("System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32),
    ("System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1),
    ("System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()", BenchmarkManagedBody.System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1),
    ("System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_Ref_System_Int32),
    ("System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32),
    ("System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()", BenchmarkManagedBody.System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T),
    ("System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32),
    ("System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()", BenchmarkManagedBody.System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1),
    ("System.Private.CoreLib/System.Memory`1::ToArray:T[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Memory_1_ToArray_T),
    ("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_Ref_System_ReadOnlySpan_1),
    ("System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_Ref_System_Span_1)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/span/memory-buffers\",");
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