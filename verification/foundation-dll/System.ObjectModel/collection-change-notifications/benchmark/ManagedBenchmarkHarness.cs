// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/collection-change-notifications
using System;
using System.Diagnostics;
using Chaos.Benchmark.collection_change_notifications;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::add_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_add_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::remove_CollectionChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_remove_CollectionChanged_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void_System_Object),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void_System_Object_System_Object),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void_System_Object_System_Object_System_Object),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void_System_Object_System_Object_System_Object_System_Object),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_Action:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_Action_System_Void),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewItems:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewItems_System_Void),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewStartingIndex:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewStartingIndex_System_Void),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldItems:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldItems_System_Void),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldStartingIndex:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldStartingIndex_System_Void),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_Action:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_Action_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewItems:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewItems_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewStartingIndex:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewStartingIndex_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldItems:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldItems_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldStartingIndex:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldStartingIndex_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::NotifyCollectionChangedEventHandler::Void(System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_NotifyCollectionChangedEventHandler_Void_System_Object_System_Object),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_BeginInvoke_System_Void_P0_P1_P2_P3),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::EndInvoke:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_EndInvoke_System_Void_P0),
    ("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::Invoke:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_Invoke_System_Void_P0_P1)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/collection-change-notifications\",");
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