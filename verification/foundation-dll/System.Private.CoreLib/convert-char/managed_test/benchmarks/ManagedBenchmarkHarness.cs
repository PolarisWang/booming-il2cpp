// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/convert/char
using System;
using System.Diagnostics;
using Chaos.Benchmark.convert_char;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32),
    ("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)", BenchmarkManagedBody.System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/convert/char\",");
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