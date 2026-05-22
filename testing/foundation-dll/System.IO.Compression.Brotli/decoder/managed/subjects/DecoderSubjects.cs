// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/decoder
// Assembly: System.IO.Compression.Brotli
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class DecoderSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Decompress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&)
    public static void Subject_0()
    {
        try { if (((default(BrotliDecoder).Decompress(default(ReadOnlySpan<byte>), default(Span<byte>), out _, out _)).GetHashCode()) != ((default(BrotliDecoder).Decompress(default(ReadOnlySpan<byte>), default(Span<byte>), out _, out _)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Dispose:System.Void()
    public static void Subject_1()
    {
        try { default(BrotliDecoder).Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::TryDecompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)
    public static void Subject_2()
    {
        try { if (((BrotliDecoder.TryDecompress(default, default, out _)) ? 1 : 0) != ((BrotliDecoder.TryDecompress(default, default, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}