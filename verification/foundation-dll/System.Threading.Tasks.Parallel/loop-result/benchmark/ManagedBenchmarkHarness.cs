// Auto-generated managed benchmark harness
// Family: family/System.Threading.Tasks.Parallel/loop-result
using System;
using System.Diagnostics;
using Chaos.Benchmark.loop_result;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::ParallelLoopResult::Void(System.Object)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_ParallelLoopResult_Void_System_Object),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Property:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Property_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method1:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method1_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Method2_System_Void_P0_P1),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::Validate:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_Validate_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Value:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Value_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Value_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Count:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Count_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Count_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_IsEmpty_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_IsEmpty_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_Key:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_get_Key_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopResult_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Threading.Tasks.Parallel\",");
Console.WriteLine("  \"familyId\": \"family/System.Threading.Tasks.Parallel/loop-result\",");
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