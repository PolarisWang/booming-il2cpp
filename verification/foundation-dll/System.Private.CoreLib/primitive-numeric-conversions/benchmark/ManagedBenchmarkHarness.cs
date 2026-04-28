// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/primitive/numeric-conversions
using System;
using System.Diagnostics;
using Chaos.Benchmark.primitive_numeric_conversions;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String),
    ("System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String),
    ("System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String),
    ("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String),
    ("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String),
    ("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String),
    ("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String),
    ("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String),
    ("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32),
    ("System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToString_System_String_System_Double),
    ("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double),
    ("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double),
    ("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String),
    ("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String),
    ("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Double_Parse_System_Double_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/primitive/numeric-conversions\",");
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