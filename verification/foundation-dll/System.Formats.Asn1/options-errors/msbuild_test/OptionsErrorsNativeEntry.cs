// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/options-errors
// Assembly: System.Formats.Asn1
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;

public static partial class OptionsErrorsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void()
    public static void Method0()
    {
        try { new AsnContentException(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String)
    public static void Method1()
    {
        try { new AsnContentException("hello"); }
        catch { _exitCode = 1; }
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String,System.Exception)
    public static void Method2()
    {
        try { new AsnContentException("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_SkipSetSortOrderVerification:System.Boolean()
    public static void Method3()
    {
        try { if (((default(AsnReaderOptions).SkipSetSortOrderVerification) ? 1 : 0) != ((default(AsnReaderOptions).SkipSetSortOrderVerification) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_UtcTimeTwoDigitYearMax:System.Int32()
    public static void Method4()
    {
        try { if (default(AsnReaderOptions).UtcTimeTwoDigitYearMax != default(AsnReaderOptions).UtcTimeTwoDigitYearMax) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean)
    public static void Method5()
    {
        try { default(AsnReaderOptions).SkipSetSortOrderVerification = true; }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32)
    public static void Method6()
    {
        try { default(AsnReaderOptions).UtcTimeTwoDigitYearMax = 42; }
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