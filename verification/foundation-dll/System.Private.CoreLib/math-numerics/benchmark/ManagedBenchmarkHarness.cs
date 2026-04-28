// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/math/numerics
using System;
using System.Diagnostics;
using Chaos.Benchmark.math_numerics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Abs_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Floor_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Round_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32),
    ("System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Sin_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_Cos_System_Double_System_Double),
    ("System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32),
    ("System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)", BenchmarkManagedBody.System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double),
    ("System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32),
    ("System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/math/numerics\",");
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