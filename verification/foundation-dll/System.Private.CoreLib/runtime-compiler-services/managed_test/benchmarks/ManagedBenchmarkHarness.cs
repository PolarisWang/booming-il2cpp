// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/runtime/compiler-services
using System;
using System.Diagnostics;
using Chaos.Benchmark.runtime_compiler_services;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeTypeHandle),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_FormattableString_System_String_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object),
    ("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/runtime/compiler-services\",");
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