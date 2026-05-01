// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/observable-collections
using System;
using System.Diagnostics;
using Chaos.Benchmark.observable_collections;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void_System_Object),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::BlockReentrancy:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_BlockReentrancy_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::CheckReentrancy:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_CheckReentrancy_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ClearItems:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ClearItems_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::InsertItem:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_InsertItem_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_Move_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::MoveItem:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_MoveItem_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnCollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnCollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnPropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnPropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::RemoveItem:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_RemoveItem_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::SetItem:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_SetItem_System_Void_P0_P1),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::ReadOnlyObservableCollection`1::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_ReadOnlyObservableCollection_1_Void_System_Object),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnCollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnCollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnPropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnPropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_get_Empty_System_Void),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::set_Empty:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_set_Empty_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/observable-collections\",");
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