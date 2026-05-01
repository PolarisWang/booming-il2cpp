// Auto-generated managed benchmark harness
// Family: family/System.Runtime.Serialization.Formatters/surrogate-selection
using System;
using System.Diagnostics;
using Chaos.Benchmark.surrogate_selection;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_GetObjectData_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_SetObjectData_System_Void_P0_P1_P2_P3),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_ChainSelector_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetNextSelector_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetSurrogate_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::SurrogateSelector::Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_SurrogateSelector_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_AddSurrogate_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_ChainSelector_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetNextSelector_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetSurrogate_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_RemoveSurrogate_System_Void_P0_P1)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.Serialization.Formatters\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.Serialization.Formatters/surrogate-selection\",");
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