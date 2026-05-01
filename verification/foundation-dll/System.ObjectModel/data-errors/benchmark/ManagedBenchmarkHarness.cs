// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/data-errors
using System;
using System.Diagnostics;
using Chaos.Benchmark.data_errors;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::DataErrorsChangedEventArgs::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_DataErrorsChangedEventArgs_Void_System_Object),
    ("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_get_PropertyName_System_Void),
    ("System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::set_PropertyName:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_DataErrorsChangedEventArgs_set_PropertyName_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_GetErrors_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_add_ErrorsChanged_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_get_HasErrors_System_Void),
    ("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_remove_ErrorsChanged_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::set_HasErrors:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_INotifyDataErrorInfo_set_HasErrors_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/data-errors\",");
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