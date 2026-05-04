// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/buffer/memory
using System;
using System.Diagnostics;
using Chaos.Benchmark.buffer_memory;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_Ref_System_Byte_Ref_System_UIntPtr),
    ("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array),
    ("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32),
    ("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_Ptr_System_Byte_Ptr_System_Int32),
    ("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_Ptr_System_Int32_System_Byte_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_Ref_System_Byte_Ref_System_UIntPtr),
    ("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_Ptr_System_Void_Ptr_System_Int64_System_Int64),
    ("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_Ptr_System_Void_Ptr_System_UInt64_System_UInt64),
    ("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)", BenchmarkManagedBody.System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/buffer/memory\",");
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