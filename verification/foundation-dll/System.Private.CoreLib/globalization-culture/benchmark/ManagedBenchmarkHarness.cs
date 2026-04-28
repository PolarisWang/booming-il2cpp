// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/globalization/culture
using System;
using System.Diagnostics;
using Chaos.Benchmark.globalization_culture;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_CurrentCulture:System.Globalization.CultureInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_CurrentCulture_System_Globalization_CultureInfo),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_InvariantCulture:System.Globalization.CultureInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_InvariantCulture_System_Globalization_CultureInfo),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::GetCultureInfo:System.Globalization.CultureInfo(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_GetCultureInfo_System_Globalization_CultureInfo_System_String),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_Name_System_String),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_DisplayName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_DisplayName_System_String),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_DateTimeFormat:System.Globalization.DateTimeFormatInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_DateTimeFormat_System_Globalization_DateTimeFormatInfo),
    ("System.Private.CoreLib/System.Globalization.CultureInfo::get_NumberFormat:System.Globalization.NumberFormatInfo()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CultureInfo_get_NumberFormat_System_Globalization_NumberFormatInfo),
    ("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String),
    ("System.Private.CoreLib/System.Globalization.CompareInfo::Compare:System.Int32(System.String,System.String,System.Globalization.CompareOptions)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CompareInfo_Compare_System_Int32_System_String_System_String_System_Globalization_CompareOptions),
    ("System.Private.CoreLib/System.Globalization.CompareInfo::IndexOf:System.Int32(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_CompareInfo_IndexOf_System_Int32_System_String_System_String),
    ("System.Private.CoreLib/System.Globalization.TextInfo::ToUpper:System.String(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_TextInfo_ToUpper_System_String_System_String),
    ("System.Private.CoreLib/System.Globalization.TextInfo::ToLower:System.String(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_TextInfo_ToLower_System_String_System_String),
    ("System.Private.CoreLib/System.Globalization.TextInfo::get_CultureName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Globalization_TextInfo_get_CultureName_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/globalization/culture\",");
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