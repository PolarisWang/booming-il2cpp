// Auto-generated managed benchmark harness
// Family: family/System.Threading.Tasks.Parallel/loop-state
using System;
using System.Diagnostics;
using Chaos.Benchmark.loop_state;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::ParallelLoopState::Void(System.Object)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_ParallelLoopState_Void_System_Object),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Property:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Property_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method1:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method1_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Method2_System_Void_P0_P1),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Validate:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_Validate_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Value:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Value_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Value_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Count:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Count_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Count_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_IsEmpty_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_IsEmpty_System_Void_P0),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_Key:System.Void()", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_get_Key_System_Void),
    ("System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Threading_Tasks_Parallel_System_Threading_Tasks_ParallelLoopState_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Threading.Tasks.Parallel\",");
Console.WriteLine("  \"familyId\": \"family/System.Threading.Tasks.Parallel/loop-state\",");
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