// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/encoder
// Assembly: System.IO.Compression.Brotli
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class EncoderSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Compress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&,System.Boolean)
    public static void Subject_0()
    {
        try { if (((default(BrotliEncoder).Compress(default(ReadOnlySpan<byte>), default(Span<byte>), out _, out _, true)).GetHashCode()) != ((default(BrotliEncoder).Compress(default(ReadOnlySpan<byte>), default(Span<byte>), out _, out _, true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Dispose:System.Void()
    public static void Subject_1()
    {
        try { default(BrotliEncoder).Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Flush:System.Buffers.OperationStatus(System.Span{System.Byte},System.Int32&)
    public static void Subject_2()
    {
        try { if (((default(BrotliEncoder).Flush(default(Span<byte>), out _)).GetHashCode()) != ((default(BrotliEncoder).Flush(default(Span<byte>), out _)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::GetMaxCompressedLength:System.Int32(System.Int32)
    public static void Subject_3()
    {
        try { if (BrotliEncoder.GetMaxCompressedLength(42) != BrotliEncoder.GetMaxCompressedLength(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)
    public static void Subject_4()
    {
        try { if (((BrotliEncoder.TryCompress(default, default, out _)) ? 1 : 0) != ((BrotliEncoder.TryCompress(default, default, out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { if (((BrotliEncoder.TryCompress(default, default, out _, 42, 42)) ? 1 : 0) != ((BrotliEncoder.TryCompress(default, default, out _, 42, 42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::.ctor:System.Void(System.Int32,System.Int32)
    public static void Subject_6()
    {
        try { new BrotliEncoder(42, 42); }
        catch { _exitCode = 1; }
    }

}