// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/delegate/core-invocation
using System;
using System.Diagnostics;
using Chaos.Benchmark.delegate_core_invocation;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate),
    ("System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate),
    ("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object),
    ("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.Delegate::get_Target:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_get_Target_System_Object),
    ("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String),
    ("System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()", BenchmarkManagedBody.System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate),
    ("System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate),
    ("System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)", BenchmarkManagedBody.System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/delegate/core-invocation\",");
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