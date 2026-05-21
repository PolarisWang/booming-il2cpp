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
        if (((default(BrotliDecoder).Decompress(default(ReadOnlySpan<byte>), default(Span<byte>), out _, out _)).GetHashCode()) != 1) _exitCode = 1;
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::Dispose:System.Void()
    public static void Subject_1()
    {
        default(BrotliDecoder).Dispose();
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliDecoder::TryDecompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)
    public static void Subject_2()
    {
        if (((BrotliDecoder.TryDecompress(default, default, out _)) ? 1 : 0) != 0) _exitCode = 1;
    }

}