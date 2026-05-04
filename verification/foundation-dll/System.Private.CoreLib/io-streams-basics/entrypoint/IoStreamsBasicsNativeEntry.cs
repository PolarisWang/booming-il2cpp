// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/io/streams/basics
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.IO;

public static class IoStreamsBasicsNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32) could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    static int Method2()
    {
        new MemoryStream().Flush();
        return (int)0;
    }

    // [3] System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    static int Method3()
    {
        return (int)(new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin));
    }

    // [4] System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    static int Method4()
    {
        new MemoryStream().SetLength(42L);
        return (int)0;
    }

    // [5] System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
    static int Method5()
    {
        return (int)(new MemoryStream().Length);
    }

    // [6] System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
    static int Method6()
    {
        return (int)(new MemoryStream().Position);
    }

    // [7] System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream) could not be auto-generated
        return 0;
    }

    // [8] System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    static int Method8()
    {
        return ((new StringReader("hello").ReadLine()).Length);
    }

    // [9] System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    static int Method9()
    {
        return ((new StringReader("hello").ReadToEnd()).Length);
    }

    // [10] System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    static int Method10()
    {
        new StringWriter().Write("hello");
        return (int)0;
    }

    // [11] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    static int Method11()
    {
        new StringWriter().WriteLine("hello");
        return (int)0;
    }

    // [12] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    static int Method12()
    {
        new StringWriter().WriteLine();
        return (int)0;
    }

    // [13] System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
    static int Method13()
    {
        // TODO: System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32() could not be auto-generated
        return 0;
    }

    // [14] System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
    static int Method14()
    {
        // TODO: System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String() could not be auto-generated
        return 0;
    }

    // [15] System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
    static int Method15()
    {
        // TODO: System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double() could not be auto-generated
        return 0;
    }

    // [16] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
    static int Method16()
    {
        // TODO: System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32) could not be auto-generated
        return 0;
    }

    // [17] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
    static int Method17()
    {
        // TODO: System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String) could not be auto-generated
        return 0;
    }

}