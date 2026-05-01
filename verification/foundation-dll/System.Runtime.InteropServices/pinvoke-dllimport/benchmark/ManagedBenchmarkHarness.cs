// Auto-generated managed benchmark harness
// Family: family/System.Runtime.InteropServices/pinvoke-dllimport
using System;
using System.Diagnostics;
using Chaos.Benchmark.pinvoke_dllimport;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::DllImportAttribute::Void(System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_DllImportAttribute_Void_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_get_Value_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_DllImportAttribute_set_Value_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::LibraryImportAttribute::Void(System.Object)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_LibraryImportAttribute_Void_System_Object),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_EntryPoint_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_LibraryName_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_SetLastError_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_StringMarshalling_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Void()", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_get_StringMarshallingCustomType_System_Void),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_EntryPoint_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_SetLastError_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_StringMarshalling_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_StringMarshallingCustomType_System_Void_P0),
    ("System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_LibraryName:System.Void(P0)", BenchmarkManagedBody.System_Runtime_InteropServices_System_Runtime_InteropServices_LibraryImportAttribute_set_LibraryName_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.InteropServices\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.InteropServices/pinvoke-dllimport\",");
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