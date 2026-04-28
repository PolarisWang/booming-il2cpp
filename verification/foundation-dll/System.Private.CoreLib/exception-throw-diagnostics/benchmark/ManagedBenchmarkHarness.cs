// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/exception/throw-diagnostics
using System;
using System.Diagnostics;
using Chaos.Benchmark.exception_throw_diagnostics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception),
    ("System.Private.CoreLib/System.Exception::get_Message:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_get_Message_System_String),
    ("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_get_InnerException_System_Exception),
    ("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_get_StackTrace_System_String),
    ("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_get_HResult_System_Int32),
    ("System.Private.CoreLib/System.Exception::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_ToString_System_String),
    ("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()", BenchmarkManagedBody.System_Private_CoreLib_System_Exception_GetBaseException_System_Exception),
    ("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String),
    ("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/exception/throw-diagnostics\",");
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