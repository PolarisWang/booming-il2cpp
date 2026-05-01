// Auto-generated managed benchmark harness
// Family: family/System.Runtime.Serialization.Formatters/binary-formatter
using System;
using System.Diagnostics;
using Chaos.Benchmark.binary_formatter;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_Void_System_Object_System_Object),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Deserialize_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Serialize_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_AssemblyFormat_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Binder_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Context_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_FilterLevel_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_SurrogateSelector_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_TypeFormat_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_AssemblyFormat_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Binder_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Context_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_FilterLevel_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_SurrogateSelector_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_TypeFormat_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.Serialization.Formatters\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.Serialization.Formatters/binary-formatter\",");
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