// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/assembly
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_assembly;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetExecutingAssembly_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetCallingAssembly_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetEntryAssembly_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetName_System_Reflection_AssemblyName),
    ("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_get_FullName_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_get_Location_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_get_ImageRuntimeVersion_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetExportedTypes_System_Type),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetForwardedTypes_System_Type),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String_System_Boolean),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetModule_System_Reflection_Module_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceStream_System_IO_Stream_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceNames_System_String),
    ("System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_Assembly_get_EntryPoint_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_AssemblyName_get_Name_System_String),
    ("System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_AssemblyName_get_Version_System_Version),
    ("System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_AssemblyName_get_CultureInfo_System_Globalization_CultureInfo),
    ("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly),
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/assembly\",");
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