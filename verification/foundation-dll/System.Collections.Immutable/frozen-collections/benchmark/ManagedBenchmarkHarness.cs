// Auto-generated managed benchmark harness
// Family: family/System.Collections.Immutable/frozen-collections
using System;
using System.Diagnostics;
using Chaos.Benchmark.frozen_collections;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::FrozenDictionary::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::FrozenSet::Void(System.Object)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Property:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method1:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Validate:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Value:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Count:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::get_Key:System.Void()", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void),
    ("System.Collections.Immutable/System.Collections.Frozen.FrozenSet::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Collections.Immutable\",");
Console.WriteLine("  \"familyId\": \"family/System.Collections.Immutable/frozen-collections\",");
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