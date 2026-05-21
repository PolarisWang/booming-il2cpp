// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/encoder
// Assembly: System.IO.Compression.Brotli
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class EncoderNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Compress:System.Buffers.OperationStatus(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32&,System.Boolean)
    public static void Method0()
    {
    var refLocal_2 = 42;
    var refLocal_3 = 42;
        try { if ((int)(BrotliEncoder.Compress(null!, null!, ref refLocal_2, ref refLocal_3, true)) != (int)(BrotliEncoder.Compress(null!, null!, ref refLocal_2, ref refLocal_3, true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Dispose:System.Void()
    public static void Method1()
    {
        try { BrotliEncoder.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::Flush:System.Buffers.OperationStatus(System.Span{System.Byte},System.Int32&)
    public static void Method2()
    {
    var refLocal_1 = 42;
        try { if ((int)(BrotliEncoder.Flush(null!, ref refLocal_1)) != (int)(BrotliEncoder.Flush(null!, ref refLocal_1))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::GetMaxCompressedLength:System.Int32(System.Int32)
    public static void Method3()
    {
        try { if (BrotliEncoder.GetMaxCompressedLength(42) != BrotliEncoder.GetMaxCompressedLength(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&)
    public static void Method4()
    {
    var refLocal_2 = 42;
        try { if (((BrotliEncoder.TryCompress(null!, null!, ref refLocal_2)) ? 1 : 0) != ((BrotliEncoder.TryCompress(null!, null!, ref refLocal_2)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::TryCompress:System.Boolean(System.ReadOnlySpan{System.Byte},System.Span{System.Byte},System.Int32&,System.Int32,System.Int32)
    public static void Method5()
    {
    var refLocal_2 = 42;
        try { if (((BrotliEncoder.TryCompress(null!, null!, ref refLocal_2, 42, 42)) ? 1 : 0) != ((BrotliEncoder.TryCompress(null!, null!, ref refLocal_2, 42, 42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliEncoder::.ctor:System.Void(System.Int32,System.Int32)
    public static void Method6()
    {
        try { new BrotliEncoder(42, 42); }
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
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}