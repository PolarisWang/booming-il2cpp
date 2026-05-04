// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/object/equality-identity
using System;
using System.Diagnostics;
using Chaos.Benchmark.object_equality_identity;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object),
    ("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object),
    ("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object),
    ("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Object_GetHashCode_System_Int32),
    ("System.Private.CoreLib/System.Object::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Object_ToString_System_String),
    ("System.Private.CoreLib/System.Object::GetType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Object_GetType_System_Type),
    ("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()", BenchmarkManagedBody.System_Private_CoreLib_System_Object_MemberwiseClone_System_Object)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/object/equality-identity\",");
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