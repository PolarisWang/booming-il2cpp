// Auto-generated native-AOT entry point
// Family: family/System.Formats.Asn1/options/errors
// Assembly: System.Formats.Asn1
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Formats.Asn1;
using System.Linq;

public static partial class OptionsErrorsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void()
    public static void Subject_0()
    {
        new AsnContentException();
    }

    // [1] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String)
    public static void Subject_1()
    {
        new AsnContentException("hello");
    }

    // [2] System.Formats.Asn1/System.Formats.Asn1.AsnContentException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_2()
    {
        new AsnContentException("hello", default);
    }

    // [3] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_SkipSetSortOrderVerification:System.Boolean()
    public static void Subject_3()
    {
        if (((default(AsnReaderOptions).SkipSetSortOrderVerification) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [4] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_UtcTimeTwoDigitYearMax:System.Int32()
    public static void Subject_4()
    {
        if (default(AsnReaderOptions).UtcTimeTwoDigitYearMax != 2049) _exitCode = 1;
    }

    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean)
    public static void Subject_5()
    {
        // TODO: System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean) could not be auto-generated
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32)
    public static void Subject_6()
    {
        // TODO: System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32) could not be auto-generated
    }

}