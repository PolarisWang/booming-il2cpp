// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/io/streams-basics
using System;
using System.Diagnostics;
using Chaos.Benchmark.io_streams_basics;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32),
    ("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_Flush_System_Void),
    ("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin),
    ("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64),
    ("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64),
    ("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64),
    ("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream),
    ("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String),
    ("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String),
    ("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String),
    ("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String),
    ("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void),
    ("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32),
    ("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String),
    ("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()", BenchmarkManagedBody.System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double),
    ("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32),
    ("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/io/streams-basics\",");
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