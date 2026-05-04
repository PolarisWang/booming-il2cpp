// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/string/char-text-core
using System;
using System.Diagnostics;
using Chaos.Benchmark.string_char_text_core;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String),
    ("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char),
    ("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Substring_System_String_System_Int32),
    ("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String),
    ("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String),
    ("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String),
    ("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])", BenchmarkManagedBody.System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object),
    ("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String),
    ("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Contains_System_Boolean_System_String),
    ("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String),
    ("System.Private.CoreLib/System.String::Split:System.String[](System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_String_Split_System_String_System_Char),
    ("System.Private.CoreLib/System.String::ToUpper:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_String_ToUpper_System_String),
    ("System.Private.CoreLib/System.String::ToLower:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_String_ToLower_System_String),
    ("System.Private.CoreLib/System.String::Trim:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_String_Trim_System_String),
    ("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])", BenchmarkManagedBody.System_Private_CoreLib_System_String_Join_System_String_System_String_System_String),
    ("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char),
    ("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char),
    ("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/string/char-text-core\",");
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