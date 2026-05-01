// Auto-generated managed benchmark harness
// Family: family/System.Linq/materialization
using System;
using System.Diagnostics;
using Chaos.Benchmark.materialization;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Linq/System.Linq.Enumerable::Enumerable::Void(System.Object)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_Enumerable_Void_System_Object),
    ("System.Linq/System.Linq.Enumerable::get_Property:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_get_Property_System_Void),
    ("System.Linq/System.Linq.Enumerable::Method1:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_Method1_System_Void_P0),
    ("System.Linq/System.Linq.Enumerable::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_Method2_System_Void_P0_P1),
    ("System.Linq/System.Linq.Enumerable::Validate:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_Validate_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::IGrouping::Void(System.Object)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_IGrouping_Void_System_Object),
    ("System.Linq/System.Linq.IGrouping::get_Property:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_get_Property_System_Void),
    ("System.Linq/System.Linq.IGrouping::Method1:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_Method1_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_Method2_System_Void_P0_P1),
    ("System.Linq/System.Linq.IGrouping::Validate:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_Validate_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::ILookup::Void(System.Object)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_ILookup_Void_System_Object),
    ("System.Linq/System.Linq.ILookup::get_Property:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_get_Property_System_Void),
    ("System.Linq/System.Linq.ILookup::Method1:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_Method1_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_Method2_System_Void_P0_P1),
    ("System.Linq/System.Linq.ILookup::Validate:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_Validate_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::IOrderedEnumerable::Void(System.Object)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_IOrderedEnumerable_Void_System_Object),
    ("System.Linq/System.Linq.IOrderedEnumerable::get_Property:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_get_Property_System_Void),
    ("System.Linq/System.Linq.IOrderedEnumerable::Method1:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_Method1_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_Method2_System_Void_P0_P1),
    ("System.Linq/System.Linq.IOrderedEnumerable::Validate:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_Validate_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::Lookup::Void(System.Object)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_Lookup_Void_System_Object),
    ("System.Linq/System.Linq.Lookup::get_Property:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_get_Property_System_Void),
    ("System.Linq/System.Linq.Lookup::Method1:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_Method1_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_Method2_System_Void_P0_P1),
    ("System.Linq/System.Linq.Lookup::Validate:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_Validate_System_Void_P0),
    ("System.Linq/System.Linq.Enumerable::get_Value:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_get_Value_System_Void),
    ("System.Linq/System.Linq.Enumerable::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_set_Value_System_Void_P0),
    ("System.Linq/System.Linq.Enumerable::get_Count:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_get_Count_System_Void),
    ("System.Linq/System.Linq.Enumerable::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_set_Count_System_Void_P0),
    ("System.Linq/System.Linq.Enumerable::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_get_IsEmpty_System_Void),
    ("System.Linq/System.Linq.Enumerable::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_set_IsEmpty_System_Void_P0),
    ("System.Linq/System.Linq.Enumerable::get_Key:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_get_Key_System_Void),
    ("System.Linq/System.Linq.Enumerable::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Enumerable_set_Key_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::get_Value:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_get_Value_System_Void),
    ("System.Linq/System.Linq.IGrouping::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_set_Value_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::get_Count:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_get_Count_System_Void),
    ("System.Linq/System.Linq.IGrouping::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_set_Count_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_get_IsEmpty_System_Void),
    ("System.Linq/System.Linq.IGrouping::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_set_IsEmpty_System_Void_P0),
    ("System.Linq/System.Linq.IGrouping::get_Key:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_get_Key_System_Void),
    ("System.Linq/System.Linq.IGrouping::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IGrouping_set_Key_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::get_Value:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_get_Value_System_Void),
    ("System.Linq/System.Linq.ILookup::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_set_Value_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::get_Count:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_get_Count_System_Void),
    ("System.Linq/System.Linq.ILookup::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_set_Count_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_get_IsEmpty_System_Void),
    ("System.Linq/System.Linq.ILookup::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_set_IsEmpty_System_Void_P0),
    ("System.Linq/System.Linq.ILookup::get_Key:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_get_Key_System_Void),
    ("System.Linq/System.Linq.ILookup::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_ILookup_set_Key_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::get_Value:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_get_Value_System_Void),
    ("System.Linq/System.Linq.IOrderedEnumerable::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_set_Value_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::get_Count:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_get_Count_System_Void),
    ("System.Linq/System.Linq.IOrderedEnumerable::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_set_Count_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_get_IsEmpty_System_Void),
    ("System.Linq/System.Linq.IOrderedEnumerable::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_set_IsEmpty_System_Void_P0),
    ("System.Linq/System.Linq.IOrderedEnumerable::get_Key:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_get_Key_System_Void),
    ("System.Linq/System.Linq.IOrderedEnumerable::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_IOrderedEnumerable_set_Key_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::get_Value:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_get_Value_System_Void),
    ("System.Linq/System.Linq.Lookup::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_set_Value_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::get_Count:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_get_Count_System_Void),
    ("System.Linq/System.Linq.Lookup::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_set_Count_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_get_IsEmpty_System_Void),
    ("System.Linq/System.Linq.Lookup::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_set_IsEmpty_System_Void_P0),
    ("System.Linq/System.Linq.Lookup::get_Key:System.Void()", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_get_Key_System_Void),
    ("System.Linq/System.Linq.Lookup::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Linq_System_Linq_Lookup_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Linq\",");
Console.WriteLine("  \"familyId\": \"family/System.Linq/materialization\",");
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