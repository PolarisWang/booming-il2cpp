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

// Auto-generated skeleton for IO Streams Basics (family/System.Private.CoreLib/io/streams-basics).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class IoStreamsBasics
{
    // Stream.Read
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Read with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32() { }

    // Stream.Write
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Write with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32() { }

    // Stream.Flush
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_Flush_System_Void()
    {
    new MemoryStream().Flush();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Flush_System_Void() { new MemoryStream().Flush(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Flush_System_Void() { new MemoryStream().Flush(); }

    // Stream.Seek
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin()
    {
    _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin() { _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin() { _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin); }

    // Stream.SetLength
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64()
    {
    new MemoryStream().SetLength(42L);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64() { new MemoryStream().SetLength(42L); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64() { new MemoryStream().SetLength(42L); }

    // Stream.get_Length
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64()
    {
    var result = new MemoryStream().Length;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64() { _ = new MemoryStream().Length; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64() { _ = new MemoryStream().Length; }

    // Stream.get_Position
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64()
    {
    var result = new MemoryStream().Position;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64() { _ = new MemoryStream().Position; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64() { _ = new MemoryStream().Position; }

    // Stream.CopyTo
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream()
    {
    // TODO: needs-manual — CopyTo with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream() { }

    // TextReader.ReadLine
    [MethodSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String()
    {
    var result = new StringReader("hello").ReadLine();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String() { _ = new StringReader("hello").ReadLine(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String() { _ = new StringReader("hello").ReadLine(); }

    // TextReader.ReadToEnd
    [MethodSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String()
    {
    var result = new StringReader("hello").ReadToEnd();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String() { _ = new StringReader("hello").ReadToEnd(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String() { _ = new StringReader("hello").ReadToEnd(); }

    // TextWriter.Write
    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String()
    {
    new StringWriter().Write("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String() { new StringWriter().Write("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String() { new StringWriter().Write("hello"); }

    // TextWriter.WriteLine
    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String()
    {
    new StringWriter().WriteLine("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String() { new StringWriter().WriteLine("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String() { new StringWriter().WriteLine("hello"); }

    // TextWriter.WriteLine
    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void()
    {
    new StringWriter().WriteLine();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void() { new StringWriter().WriteLine(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void() { new StringWriter().WriteLine(); }

    // BinaryReader.ReadInt32
    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32()
    {
    // TODO: needs-manual — ReadInt32 with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32() { }

    // BinaryReader.ReadString
    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String()
    {
    // TODO: needs-manual — ReadString with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String() { }

    // BinaryReader.ReadDouble
    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double()
    {
    // TODO: needs-manual — ReadDouble with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double() { }

    // BinaryWriter.Write
    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32()
    {
    // TODO: needs-manual — Write with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32() { }

    // BinaryWriter.Write
    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String()
    {
    // TODO: needs-manual — Write with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    public static void Benchmark_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String() { }
}
