// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/threading/monitor-interlocked
using System;
using System.Diagnostics;
using Chaos.Benchmark.threading_monitor_interlocked;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object),
    ("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object),
    ("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32),
    ("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object),
    ("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object),
    ("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32_Ref),
    ("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32_Ref),
    ("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_Ref_System_Int32),
    ("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_Ref_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_Ref_System_Int32),
    ("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void),
    ("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)", BenchmarkManagedBody.System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32_Ref)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/threading/monitor-interlocked\",");
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