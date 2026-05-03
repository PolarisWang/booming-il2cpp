// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/activation
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_activation;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type),
    ("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Object_Array),
    ("System.Private.CoreLib/System.Activator::CreateInstance:T()", BenchmarkManagedBody.System_Private_CoreLib_System_Activator_CreateInstance_T),
    ("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)", BenchmarkManagedBody.System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object_Array_System_Globalization_CultureInfo),
    ("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32),
    ("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])", BenchmarkManagedBody.System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_Array),
    ("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object_Array)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/activation\",");
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