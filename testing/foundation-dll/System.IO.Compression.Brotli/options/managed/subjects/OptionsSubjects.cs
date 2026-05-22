// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/options
// Assembly: System.IO.Compression.Brotli
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;

public static partial class OptionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Quality:System.Int32()
    public static void Subject_0()
    {
        try { if (default(BrotliCompressionOptions)!.Quality != default(BrotliCompressionOptions)!.Quality) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Quality:System.Void(System.Int32)
    public static void Subject_1()
    {
        try { default(BrotliCompressionOptions)!.Quality = 42; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::.ctor:System.Void()
    public static void Subject_2()
    {
        try { new BrotliCompressionOptions(); }
        catch { _exitCode = 1; }
    }

}