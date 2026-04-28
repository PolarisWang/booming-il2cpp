// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/time/date-time-timespan
using System;
using System.Diagnostics;
using Chaos.Benchmark.time_date_time_timespan;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String),
    ("System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime_Ref),
    ("System.Private.CoreLib/System.DateTime::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_ToString_System_String),
    ("System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_ToString_System_String_System_String),
    ("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double),
    ("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double),
    ("System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double),
    ("System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime),
    ("System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double),
    ("System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double),
    ("System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double),
    ("System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/time/date-time-timespan\",");
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