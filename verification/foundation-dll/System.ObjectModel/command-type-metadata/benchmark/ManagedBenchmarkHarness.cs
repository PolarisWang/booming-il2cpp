// Auto-generated managed benchmark harness
// Family: family/System.ObjectModel/command-type-metadata
using System;
using System.Diagnostics;
using Chaos.Benchmark.command_type_metadata;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void),
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_TypeConverterAttribute_Void_System_Object),
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_Equals_System_Void_P0),
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_GetHashCode_System_Void),
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_get_ConverterTypeName_System_Void),
    ("System.ObjectModel/System.ComponentModel.TypeConverterAttribute::set_ConverterTypeName:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_ComponentModel_TypeConverterAttribute_set_ConverterTypeName_System_Void_P0),
    ("System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Reflection_ICustomTypeProvider_GetCustomType_System_Void),
    ("System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Input_ICommand_CanExecute_System_Void_P0),
    ("System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Input_ICommand_Execute_System_Void_P0),
    ("System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Input_ICommand_add_CanExecuteChanged_System_Void_P0),
    ("System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Input_ICommand_remove_CanExecuteChanged_System_Void_P0),
    ("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::ValueSerializerAttribute::Void(System.Object)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_ValueSerializerAttribute_Void_System_Object),
    ("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerType_System_Void),
    ("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.Void()", BenchmarkManagedBody.System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_get_ValueSerializerTypeName_System_Void),
    ("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerType:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerType_System_Void_P0),
    ("System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerTypeName:System.Void(P0)", BenchmarkManagedBody.System_ObjectModel_System_Windows_Markup_ValueSerializerAttribute_set_ValueSerializerTypeName_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.ObjectModel\",");
Console.WriteLine("  \"familyId\": \"family/System.ObjectModel/command-type-metadata\",");
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