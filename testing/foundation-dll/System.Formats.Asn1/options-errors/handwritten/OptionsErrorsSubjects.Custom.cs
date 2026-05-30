// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class OptionsErrorsSubjects
{
    // [5] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_SkipSetSortOrderVerification:System.Void(System.Boolean)
    public static void CustomEntrySubject_5()
    {
        try {
            default(AsnReaderOptions).SkipSetSortOrderVerification = true;
        }
        catch { _exitCode = 1; }
    }

    // [6] System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_UtcTimeTwoDigitYearMax:System.Void(System.Int32)
    public static void CustomEntrySubject_6()
    {
        try {
            default(AsnReaderOptions).UtcTimeTwoDigitYearMax = 42;
        }
        catch { _exitCode = 1; }
    }

}
