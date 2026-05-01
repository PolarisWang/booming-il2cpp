// Auto-generated managed benchmark harness
// Family: family/System.Collections.Immutable/extensions-marshal
using System;
using System.Diagnostics;
using Chaos.Benchmark.extensions_marshal;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::ImmutableArrayExtensions::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_ImmutableArrayExtensions_Void_System_Object),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Property_System_Void),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::ImmutableCollectionsMarshal::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_ImmutableCollectionsMarshal_Void_System_Object),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Property_System_Void),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Value_System_Void),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Count_System_Void),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_get_Key_System_Void),
    ("System.Collections.Immutable/System.Linq.ImmutableArrayExtensions::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Linq_ImmutableArrayExtensions_set_Key_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Value_System_Void),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Count_System_Void),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_get_Key_System_Void),
    ("System.Collections.Immutable/System.Runtime.InteropServices.ImmutableCollectionsMarshal::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Runtime_InteropServices_ImmutableCollectionsMarshal_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Collections.Immutable\",");
Console.WriteLine("  \"familyId\": \"family/System.Collections.Immutable/extensions-marshal\",");
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