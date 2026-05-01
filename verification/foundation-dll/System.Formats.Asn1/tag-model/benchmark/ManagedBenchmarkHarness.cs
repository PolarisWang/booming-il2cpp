// Auto-generated managed benchmark harness
// Family: family/System.Formats.Asn1/tag-model
using System;
using System.Diagnostics;
using Chaos.Benchmark.tag_model;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Asn1Tag::Void(System.Object)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Asn1Tag_Void_System_Object),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Property:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Property_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Method1:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Method1_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Method2_System_Void_P0_P1),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::Validate:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_Validate_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::TagClass::Void(System.Object)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_TagClass_Void_System_Object),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Property:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Property_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::Method1:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_Method1_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_Method2_System_Void_P0_P1),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::Validate:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_Validate_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::UniversalTagNumber::Void(System.Object)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_UniversalTagNumber_Void_System_Object),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Property:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Property_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Method1:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Method1_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Method2:System.Void(P0, P1)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Method2_System_Void_P0_P1),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::Validate:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_Validate_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Value:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Value_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Value_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Count:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Count_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Count_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_IsEmpty_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_IsEmpty_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::get_Key:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_get_Key_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.Asn1Tag::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_Asn1Tag_set_Key_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Value:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Value_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Value_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Count:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Count_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Count_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_get_IsEmpty_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_set_IsEmpty_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::get_Key:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_get_Key_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.TagClass::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_TagClass_set_Key_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Value:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Value_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Value:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Value_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Count:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Count_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Count:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Count_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_IsEmpty:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_IsEmpty_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_IsEmpty:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_IsEmpty_System_Void_P0),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::get_Key:System.Void()", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_get_Key_System_Void),
    ("System.Formats.Asn1/System.Formats.Asn1.UniversalTagNumber::set_Key:System.Void(P0)", BenchmarkManagedBody.System_Formats_Asn1_System_Formats_Asn1_UniversalTagNumber_set_Key_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Formats.Asn1\",");
Console.WriteLine("  \"familyId\": \"family/System.Formats.Asn1/tag-model\",");
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