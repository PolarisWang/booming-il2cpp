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

// Auto-generated hot-update skeletons for IoStreamsBasicsHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class IoStreamsBasicsHotUpdate
{
    // Purpose: Verify Stream.Read executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32() { }
    // Purpose: Verify Stream.Write executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32() { }
    // Purpose: Verify Stream.Flush executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Flush_System_Void() { new MemoryStream().Flush(); }
    // Purpose: Verify Stream.Seek executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin() { _ = new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin); }
    // Purpose: Verify Stream.SetLength executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64() { new MemoryStream().SetLength(42L); }
    // Purpose: Verify Stream.get_Length executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64() { _ = new MemoryStream().Length; }
    // Purpose: Verify Stream.get_Position executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64() { _ = new MemoryStream().Position; }
    // Purpose: Verify Stream.CopyTo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream() { }
    // Purpose: Verify TextReader.ReadLine executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String() { _ = new StringReader("hello").ReadLine(); }
    // Purpose: Verify TextReader.ReadToEnd executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String() { _ = new StringReader("hello").ReadToEnd(); }
    // Purpose: Verify TextWriter.Write executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String() { new StringWriter().Write("hello"); }
    // Purpose: Verify TextWriter.WriteLine executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String() { new StringWriter().WriteLine("hello"); }
    // Purpose: Verify TextWriter.WriteLine executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void() { new StringWriter().WriteLine(); }
    // Purpose: Verify BinaryReader.ReadInt32 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32() { }
    // Purpose: Verify BinaryReader.ReadString executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String() { }
    // Purpose: Verify BinaryReader.ReadDouble executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double() { }
    // Purpose: Verify BinaryWriter.Write executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32() { }
    // Purpose: Verify BinaryWriter.Write executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String() { }}
