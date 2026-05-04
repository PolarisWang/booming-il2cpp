// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/collections/generic-core
using System;
using System.Diagnostics;
using Chaos.Benchmark.collections_generic_core;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void),
    ("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T),
    ("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue),
    ("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32),
    ("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue_Ref),
    ("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey),
    ("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey),
    ("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T),
    ("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T),
    ("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)", BenchmarkManagedBody.System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/collections/generic-core\",");
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