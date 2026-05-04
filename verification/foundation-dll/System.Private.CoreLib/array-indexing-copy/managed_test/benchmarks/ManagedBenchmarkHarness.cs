// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/array/indexing-copy
using System;
using System.Diagnostics;
using Chaos.Benchmark.array_indexing_copy;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32),
    ("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Resize_System_Void_T_Ref_System_Int32),
    ("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Sort_System_Void_System_Array),
    ("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer),
    ("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object),
    ("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object),
    ("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object),
    ("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object),
    ("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array),
    ("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/array/indexing-copy\",");
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