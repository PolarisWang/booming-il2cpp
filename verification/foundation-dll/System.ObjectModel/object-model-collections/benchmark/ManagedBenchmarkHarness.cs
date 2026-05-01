// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/object-model-collections
using System;
using System.Diagnostics;
using Chaos.Benchmark.object_model_collections;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void_System_Object),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::KeyedCollection`2::Void(System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_KeyedCollection_2_Void_System_Object_System_Object),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::ChangeItemKey:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_ChangeItemKey_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::ClearItems:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_ClearItems_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Contains:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_Contains_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::GetKeyForItem:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_GetKeyForItem_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::InsertItem:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_InsertItem_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::Remove:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_Remove_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::RemoveItem:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_RemoveItem_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::SetItem:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_SetItem_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::TryGetValue:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_TryGetValue_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Comparer:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Comparer_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Dictionary:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Dictionary_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Item_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Comparer:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Comparer_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Dictionary:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Dictionary_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::get_Item:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_get_Item_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.KeyedCollection`2::set_Item:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_KeyedCollection_2_set_Item_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/object-model-collections\",");
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