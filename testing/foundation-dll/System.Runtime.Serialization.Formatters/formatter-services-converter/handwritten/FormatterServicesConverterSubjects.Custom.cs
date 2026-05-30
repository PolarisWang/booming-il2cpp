// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class FormatterServicesConverterSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.Type)
    public static void CustomEntrySubject_0()
    {
        try {
            Assert.IsNotNull(default(FormatterConverter)!.Convert(null!, typeof(byte)));
        }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.TypeCode)
    public static void CustomEntrySubject_1()
    {
        try {
            Assert.IsNotNull(default(FormatterConverter)!.Convert(null!, default));
        }
        catch { _exitCode = 1; }
    }

}
