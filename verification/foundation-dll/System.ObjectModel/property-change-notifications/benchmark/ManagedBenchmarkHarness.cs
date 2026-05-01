// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/property-change-notifications
using System;
using System.Diagnostics;
using Chaos.Benchmark.property_change_notifications;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyPropertyChanged_add_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyPropertyChanged_remove_PropertyChanged_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyPropertyChanging_add_PropertyChanging_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyPropertyChanging_remove_PropertyChanging_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::PropertyChangedEventArgs::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_PropertyChangedEventArgs_Void_System_Object),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::get_PropertyName:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_get_PropertyName_System_Void),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::set_PropertyName:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventArgs_set_PropertyName_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::PropertyChangedEventHandler::Void(System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_PropertyChangedEventHandler_Void_System_Object_System_Object),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_BeginInvoke_System_Void_P0_P1_P2_P3),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::EndInvoke:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_EndInvoke_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::Invoke:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangedEventHandler_Invoke_System_Void_P0_P1),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::PropertyChangingEventArgs::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_PropertyChangingEventArgs_Void_System_Object),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::get_PropertyName:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_get_PropertyName_System_Void),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::set_PropertyName:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventArgs_set_PropertyName_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::PropertyChangingEventHandler::Void(System.Object, System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_PropertyChangingEventHandler_Void_System_Object_System_Object),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_BeginInvoke_System_Void_P0_P1_P2_P3),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::EndInvoke:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_EndInvoke_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::Invoke:System.Void(P0, P1)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_PropertyChangingEventHandler_Invoke_System_Void_P0_P1)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/property-change-notifications\",");
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