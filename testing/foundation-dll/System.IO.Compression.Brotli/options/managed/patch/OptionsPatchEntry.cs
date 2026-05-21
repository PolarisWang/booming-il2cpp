// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/options
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class OptionsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::get_Quality:System.Int32()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::set_Quality:System.Void(System.Int32)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliCompressionOptions::.ctor:System.Void()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}