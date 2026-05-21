// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/options/errors
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class OptionsErrorsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String,System.Exception)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_SkipSetSortOrderVerification:System.Boolean()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_UtcTimeTwoDigitYearMax:System.Int32()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
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
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}