// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/decoder
// Assembly: System.IO.Compression.Brotli
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class DecoderNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Decompress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&)
    public static void Method0()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
        try { if ((int)(default(BrotliDecoder).Decompress(null!, null!, out refLocal_2, out refLocal_3)) != (int)(default(BrotliDecoder).Decompress(null!, null!, out refLocal_2, out refLocal_3))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Dispose:System.Void()
    public static void Method1()
    {
        try { default(BrotliDecoder).Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::TryDecompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)
    public static void Method2()
    {
    var refLocal_2 = 42;
        try { if (((default(BrotliDecoder).TryDecompress(null!, null!, out refLocal_2)) ? 1 : 0) != ((default(BrotliDecoder).TryDecompress(null!, null!, out refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}