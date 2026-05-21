// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/options
// Assembly: System.IO.Compression.Brotli
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class OptionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Quality:System.Int32()
    public static void Method0()
    {
        try { if (BrotliCompressionOptions.Quality != BrotliCompressionOptions.Quality) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Quality:System.Void(System.Int32)
    public static void Method1()
    {
        try { BrotliCompressionOptions.Quality = 42; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::.ctor:System.Void()
    public static void Method2()
    {
        try { new BrotliCompressionOptions(); }
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