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
    // [0] System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = new MemoryStream(new byte[10]).Read(new byte[5], 0, 5); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
    [Fact]
    public static void Subject_1()
    {
        try { new MemoryStream().Write(new byte[5], 0, 5); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    [Fact]
    public static void Subject_2()
    {
        try { new MemoryStream().Flush(); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = (int)(new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    [Fact]
    public static void Subject_4()
    {
        try { new MemoryStream().SetLength(42L); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = (int)(new MemoryStream().Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = (int)(new MemoryStream().Position); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
    [Fact]
    public static void Subject_7()
    {
        try { new MemoryStream().CopyTo(new MemoryStream()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((new StringReader("hello").ReadLine()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((new StringReader("hello").ReadToEnd()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    [Fact]
    public static void Subject_10()
    {
        try { new StringWriter().Write("hello"); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    [Fact]
    public static void Subject_11()
    {
        try { new StringWriter().WriteLine("hello"); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    [Fact]
    public static void Subject_12()
    {
        try { new StringWriter().WriteLine(); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
    [Fact]
    public static void Subject_13()
    {
        try { var __val = new BinaryReader(new MemoryStream(new byte[4])).ReadInt32(); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [14] System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
    [Fact]
    public static void Subject_14()
    {
        try { var __val = ((new BinaryReader(new MemoryStream(new byte[] { 0 })).ReadString()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [15] System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
    [Fact]
    public static void Subject_15()
    {
        try { var __val = (int)(new BinaryReader(new MemoryStream(new byte[8])).ReadDouble()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [16] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
    [Fact]
    public static void Subject_16()
    {
        try { new BinaryWriter(new MemoryStream()).Write(42); }
        catch { }
    }

    // [17] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
    [Fact]
    public static void Subject_17()
    {
        try { new BinaryWriter(new MemoryStream()).Write(42); }
        catch { }
    }

}