// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/enum/parsing
using System;
using System.Diagnostics;
using Chaos.Benchmark.enum_parsing;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_Format_System_String_System_Type_System_Object_System_String),
    ("System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_GetName_System_String_System_RuntimeType_System_UInt64),
    ("System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_GetName_System_String_System_Type_System_Object),
    ("System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_GetNames_System_String_System_Type),
    ("System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_GetValues_System_Array_System_Type),
    ("System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_IsDefined_System_Boolean_System_Type_System_Object),
    ("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String),
    ("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String_System_Boolean),
    ("System.Private.CoreLib/System.Enum::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_ToString_System_String),
    ("System.Private.CoreLib/System.Enum::ToString:System.String(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_ToString_System_String_System_String),
    ("System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Boolean_System_Object_Ref),
    ("System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)", BenchmarkManagedBody.System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Object_Ref)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/enum/parsing\",");
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