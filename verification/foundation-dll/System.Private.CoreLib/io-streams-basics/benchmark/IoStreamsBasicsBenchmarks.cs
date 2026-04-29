using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated benchmark skeletons for IoStreamsBasicsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class IoStreamsBasicsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Stream.Read with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32() { _ = new MemoryStream().Read(new byte[] { 1, 2, 3 }, 42, 42); }
    // Purpose: Benchmark native-runtime performance of Stream.Write with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32() { new MemoryStream().Write(new byte[] { 1, 2, 3 }, 42, 42); }
    // Purpose: Benchmark native-runtime performance of Stream.Flush with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Flush_System_Void() { new MemoryStream().Flush(); }
    // Purpose: Benchmark native-runtime performance of Stream.Seek with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin() { _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin); }
    // Purpose: Benchmark native-runtime performance of Stream.SetLength with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64() { new MemoryStream().SetLength(42L); }
    // Purpose: Benchmark native-runtime performance of Stream.get_Length with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64() { _ = new MemoryStream().Length; }
    // Purpose: Benchmark native-runtime performance of Stream.get_Position with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64() { _ = new MemoryStream().Position; }
    // Purpose: Benchmark native-runtime performance of Stream.CopyTo with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream() { new MemoryStream().CopyTo(null!); }
    // Purpose: Benchmark native-runtime performance of TextReader.ReadLine with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String() { _ = new StringReader("hello").ReadLine(); }
    // Purpose: Benchmark native-runtime performance of TextReader.ReadToEnd with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String() { _ = new StringReader("hello").ReadToEnd(); }
    // Purpose: Benchmark native-runtime performance of TextWriter.Write with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String() { new StringWriter().Write("hello"); }
    // Purpose: Benchmark native-runtime performance of TextWriter.WriteLine with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String() { new StringWriter().WriteLine("hello"); }
    // Purpose: Benchmark native-runtime performance of TextWriter.WriteLine with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void() { new StringWriter().WriteLine(); }
    // Purpose: Benchmark native-runtime performance of BinaryReader.ReadInt32 with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32() { _ = default(BinaryReader)!.ReadInt32(); }
    // Purpose: Benchmark native-runtime performance of BinaryReader.ReadString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String() { _ = default(BinaryReader)!.ReadString(); }
    // Purpose: Benchmark native-runtime performance of BinaryReader.ReadDouble with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double() { _ = default(BinaryReader)!.ReadDouble(); }
    // Purpose: Benchmark native-runtime performance of BinaryWriter.Write with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32() { default(BinaryWriter)!.Write(42); }
    // Purpose: Benchmark native-runtime performance of BinaryWriter.Write with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String() { default(BinaryWriter)!.Write("hello"); }}
