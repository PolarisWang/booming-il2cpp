// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/guid/random/hashcode
using System;
using System.Diagnostics;
using Chaos.Benchmark.guid_random_hashcode;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_ctor_System_Void_System_String),
    ("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte),
    ("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_NewGuid_System_Guid),
    ("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String),
    ("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid_Ref),
    ("System.Private.CoreLib/System.Guid::ToString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Guid_ToString_System_String),
    ("System.Private.CoreLib/System.Random::.ctor:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_Random_ctor_System_Void),
    ("System.Private.CoreLib/System.Random::Next:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_Random_Next_System_Int32),
    ("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Random::NextDouble:System.Double()", BenchmarkManagedBody.System_Private_CoreLib_System_Random_NextDouble_System_Double),
    ("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])", BenchmarkManagedBody.System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte),
    ("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)", BenchmarkManagedBody.System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object),
    ("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32),
    ("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/guid/random/hashcode\",");
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