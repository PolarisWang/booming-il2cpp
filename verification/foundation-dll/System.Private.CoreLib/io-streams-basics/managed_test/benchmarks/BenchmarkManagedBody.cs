// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/io/streams-basics
using System;

namespace Chaos.Benchmark.io_streams_basics
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_IO_Stream_Read_System_Int32_System_Byte_System_Int32_System_Int32()
        {
            BenchmarkChecksum += new MemoryStream().Read(new byte[] { 1, 2, 3 }, 42, 42);
        }

        // System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_IO_Stream_Write_System_Void_System_Byte_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(new MemoryStream().Write(new byte[] { 1, 2, 3 }, 42, 42));
        }

        // System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
        public static void System_Private_CoreLib_System_IO_Stream_Flush_System_Void()
        {
            BenchmarkChecksum += (int)(new MemoryStream().Flush());
        }

        // System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
        public static void System_Private_CoreLib_System_IO_Stream_Seek_System_Int64_System_Int64_System_IO_SeekOrigin()
        {
            BenchmarkChecksum += (int)(new MemoryStream().Seek(42L, System.IO.SeekOrigin.Begin));
        }

        // System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
        public static void System_Private_CoreLib_System_IO_Stream_SetLength_System_Void_System_Int64()
        {
            BenchmarkChecksum += (int)(new MemoryStream().SetLength(42L));
        }

        // System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
        public static void System_Private_CoreLib_System_IO_Stream_get_Length_System_Int64()
        {
            BenchmarkChecksum += (int)(new MemoryStream().Length);
        }

        // System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
        public static void System_Private_CoreLib_System_IO_Stream_get_Position_System_Int64()
        {
            BenchmarkChecksum += (int)(new MemoryStream().Position);
        }

        // System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
        public static void System_Private_CoreLib_System_IO_Stream_CopyTo_System_Void_System_IO_Stream()
        {
            BenchmarkChecksum += (int)(new MemoryStream().CopyTo(null!));
        }

        // System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
        public static void System_Private_CoreLib_System_IO_TextReader_ReadLine_System_String()
        {
            BenchmarkChecksum += ((new StringReader("hello").ReadLine()).Length);
        }

        // System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
        public static void System_Private_CoreLib_System_IO_TextReader_ReadToEnd_System_String()
        {
            BenchmarkChecksum += ((new StringReader("hello").ReadToEnd()).Length);
        }

        // System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
        public static void System_Private_CoreLib_System_IO_TextWriter_Write_System_Void_System_String()
        {
            BenchmarkChecksum += (int)(new StringWriter().Write("hello"));
        }

        // System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
        public static void System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void_System_String()
        {
            BenchmarkChecksum += (int)(new StringWriter().WriteLine("hello"));
        }

        // System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
        public static void System_Private_CoreLib_System_IO_TextWriter_WriteLine_System_Void()
        {
            BenchmarkChecksum += (int)(new StringWriter().WriteLine());
        }

        // System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
        public static void System_Private_CoreLib_System_IO_BinaryReader_ReadInt32_System_Int32()
        {
            BenchmarkChecksum += default(BinaryReader)!.ReadInt32();
        }

        // System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
        public static void System_Private_CoreLib_System_IO_BinaryReader_ReadString_System_String()
        {
            BenchmarkChecksum += ((default(BinaryReader)!.ReadString()).Length);
        }

        // System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
        public static void System_Private_CoreLib_System_IO_BinaryReader_ReadDouble_System_Double()
        {
            BenchmarkChecksum += (int)(default(BinaryReader)!.ReadDouble());
        }

        // System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
        public static void System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_Int32()
        {
            BenchmarkChecksum += (int)(default(BinaryWriter)!.Write(42));
        }

        // System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
        public static void System_Private_CoreLib_System_IO_BinaryWriter_Write_System_Void_System_String()
        {
            BenchmarkChecksum += (int)(default(BinaryWriter)!.Write("hello"));
        }

    }
}
