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
using Xunit;

// Auto-generated skeleton for IO Streams Basics (family/System.Private.CoreLib/io/streams-basics).
public partial class IoStreamsBasicsTests
{
    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Read with 3 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Read with 3 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Write with 3 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Write with 3 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_Stream_Flush_System_Void()
    {
    new MemoryStream().Flush();
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin()
    {
    _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64()
    {
    new MemoryStream().SetLength(42L);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64()
    {
    var result = new MemoryStream().Length;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64()
    {
    var result = new MemoryStream().Position;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — CopyTo with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream()
    {
    // TODO: needs-manual — CopyTo with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String()
    {
    var result = new StringReader("hello").ReadLine();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String()
    {
    var result = new StringReader("hello").ReadToEnd();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String()
    {
    new StringWriter().Write("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String()
    {
    new StringWriter().WriteLine("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void()
    {
    new StringWriter().WriteLine();
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ReadInt32 with 0 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32()
    {
    // TODO: needs-manual — ReadInt32 with 0 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ReadString with 0 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String()
    {
    // TODO: needs-manual — ReadString with 0 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ReadDouble with 0 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double()
    {
    // TODO: needs-manual — ReadDouble with 0 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Write with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32()
    {
    // TODO: needs-manual — Write with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Write with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String()
    {
    // TODO: needs-manual — Write with 1 params requires manual implementation
    }}
