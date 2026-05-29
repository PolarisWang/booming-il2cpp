// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/io/streams-basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class IoStreamsBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
    public static void Subject_0()
    {
        try { _ = new MemoryStream(new byte[10]).Read(new byte[5], 0, 5); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
    public static void Subject_1()
    {
        try { new MemoryStream().Write(new byte[5], 0, 5); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    public static void Subject_2()
    {
        // non-callable: System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    }

    // [3] System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    }

    // [4] System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    public static void Subject_4()
    {
        // non-callable: System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    }

    // [5] System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
    public static void Subject_5()
    {
        try { _ = (int)(new MemoryStream().Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
    public static void Subject_6()
    {
        try { _ = (int)(new MemoryStream().Position); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
    public static void Subject_7()
    {
        try { new MemoryStream().CopyTo(new MemoryStream()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    public static void Subject_8()
    {
        // non-callable: System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    }

    // [9] System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    public static void Subject_9()
    {
        // non-callable: System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    }

    // [10] System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    public static void Subject_10()
    {
        // non-callable: System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    }

    // [11] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    public static void Subject_11()
    {
        // non-callable: System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    }

    // [12] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    public static void Subject_12()
    {
        // non-callable: System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    }

    // [13] System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
    public static void Subject_13()
    {
        try { _ = new BinaryReader(new MemoryStream(new byte[4])).ReadInt32(); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
    public static void Subject_14()
    {
        try { _ = ((new BinaryReader(new MemoryStream(new byte[] { 0 })).ReadString()).Length); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
    public static void Subject_15()
    {
        try { _ = (int)(new BinaryReader(new MemoryStream(new byte[8])).ReadDouble()); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
    public static void Subject_16()
    {
        try { new BinaryWriter(new MemoryStream()).Write(42); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
    public static void Subject_17()
    {
        try { new BinaryWriter(new MemoryStream()).Write(42); }
        catch { _exitCode = 1; }
    }

}