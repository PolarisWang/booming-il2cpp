// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/io/streams/basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

public static partial class IoStreamsBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
    public static void Subject_0()
    {
        if (new MemoryStream(new byte[10]).Read(new byte[5], 0, 5) != 5) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
    public static void Subject_1()
    {
        new MemoryStream().Write(new byte[5], 0, 5);
    }

    // [2] System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    public static void Subject_2()
    {
        new MemoryStream().Flush();
    }

    // [3] System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    public static void Subject_3()
    {
        if ((int)(new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin)) != 42) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    public static void Subject_4()
    {
        new MemoryStream().SetLength(42L);
    }

    // [5] System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
    public static void Subject_5()
    {
        if ((int)(new MemoryStream().Length) != 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
    public static void Subject_6()
    {
        if ((int)(new MemoryStream().Position) != 0) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
    public static void Subject_7()
    {
        new MemoryStream().CopyTo(new MemoryStream());
    }

    // [8] System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    public static void Subject_8()
    {
        if (((new StringReader("hello").ReadLine()).Length) != 5) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    public static void Subject_9()
    {
        if (((new StringReader("hello").ReadToEnd()).Length) != 5) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    public static void Subject_10()
    {
        new StringWriter().Write("hello");
    }

    // [11] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    public static void Subject_11()
    {
        new StringWriter().WriteLine("hello");
    }

    // [12] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    public static void Subject_12()
    {
        new StringWriter().WriteLine();
    }

    // [13] System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
    public static void Subject_13()
    {
        if (new BinaryReader(new MemoryStream(new byte[4])).ReadInt32() != 0) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
    public static void Subject_14()
    {
        if (((new BinaryReader(new MemoryStream(new byte[] { 0 })).ReadString()).Length) != 0) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
    public static void Subject_15()
    {
        if ((int)(new BinaryReader(new MemoryStream(new byte[8])).ReadDouble()) != 0) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
    public static void Subject_16()
    {
        new BinaryWriter(new MemoryStream()).Write(42);
    }

    // [17] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
    public static void Subject_17()
    {
        new BinaryWriter(new MemoryStream()).Write(42);
    }

}