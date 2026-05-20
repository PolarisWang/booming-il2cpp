using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/io/streams-basics
public static class IoStreamsBasicsPatch
{
    // Verify Stream.Read after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32() { _ = new MemoryStream(new byte[10]).Read(new byte[5], 0, 5); }
    // Verify Stream.Write after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32() { new MemoryStream().Write(new byte[5], 0, 5); }
    // Verify Stream.Flush after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_Flush_System_Void() { new MemoryStream().Flush(); }
    // Verify Stream.Seek after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin() { _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin); }
    // Verify Stream.SetLength after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64() { new MemoryStream().SetLength(42L); }
    // Verify Stream.get_Length after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64() { _ = new MemoryStream().Length; }
    // Verify Stream.get_Position after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64() { _ = new MemoryStream().Position; }
    // Verify Stream.CopyTo after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream() { new MemoryStream().CopyTo(new MemoryStream()); }
    // Verify TextReader.ReadLine after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String() { _ = new StringReader("hello").ReadLine(); }
    // Verify TextReader.ReadToEnd after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String() { _ = new StringReader("hello").ReadToEnd(); }
    // Verify TextWriter.Write after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String() { new StringWriter().Write("hello"); }
    // Verify TextWriter.WriteLine after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String() { new StringWriter().WriteLine("hello"); }
    // Verify TextWriter.WriteLine after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void() { new StringWriter().WriteLine(); }
    // Verify BinaryReader.ReadInt32 after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32() { _ = new BinaryReader(new MemoryStream(new byte[4])).ReadInt32(); }
    // Verify BinaryReader.ReadString after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String() { _ = new BinaryReader(new MemoryStream(new byte[] { 0 })).ReadString(); }
    // Verify BinaryReader.ReadDouble after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double() { _ = new BinaryReader(new MemoryStream(new byte[8])).ReadDouble(); }
    // Verify BinaryWriter.Write after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32() { new BinaryWriter(new MemoryStream()).Write(42); }
    // Verify BinaryWriter.Write after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String() { new BinaryWriter(new MemoryStream()).Write(42); }}
