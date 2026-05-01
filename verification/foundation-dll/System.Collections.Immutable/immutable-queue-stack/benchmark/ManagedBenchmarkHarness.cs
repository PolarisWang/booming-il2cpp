// Auto-generated managed benchmark harness
// Family: family/System.Collections.Immutable/immutable-queue-stack
using System;
using System.Diagnostics;
using Chaos.Benchmark.immutable_queue_stack;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::ImmutableQueue::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_ImmutableQueue_Void_System_Object),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Property_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::ImmutableStack::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_ImmutableStack_Void_System_Object),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Property_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Value_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Count_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Key_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Key_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Value_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Count_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Key_System_Void),
    ("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Collections.Immutable\",");
Console.WriteLine("  \"familyId\": \"family/System.Collections.Immutable/immutable-queue-stack\",");
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