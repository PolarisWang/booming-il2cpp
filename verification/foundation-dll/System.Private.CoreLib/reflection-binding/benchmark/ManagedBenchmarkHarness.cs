// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/binding
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_binding;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String),
    ("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethod_System_Reflection_MethodInfo_System_String_System_Type),
    ("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String),
    ("System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetField_System_Reflection_FieldInfo_System_String_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String),
    ("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetProperty_System_Reflection_PropertyInfo_System_String_System_Type),
    ("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String),
    ("System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetEvent_System_Reflection_EventInfo_System_String_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object),
    ("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)", BenchmarkManagedBody.System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Reflection_BindingFlags_System_Reflection_Binder_System_Object_System_Globalization_CultureInfo)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/binding\",");
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