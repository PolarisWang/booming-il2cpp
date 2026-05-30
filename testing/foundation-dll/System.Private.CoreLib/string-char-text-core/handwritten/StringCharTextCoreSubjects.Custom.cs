// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class StringCharTextCoreSubjects
{
    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    public static void CustomEntrySubject_17()
    {        try { _ = Char.IsDigit('A'); } catch { _exitCode = 1; }    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    public static void CustomEntrySubject_18()
    {        try { _ = Char.IsLetter('A'); } catch { _exitCode = 1; }    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    public static void CustomEntrySubject_19()
    {        try { _ = Char.IsWhiteSpace('A'); } catch { _exitCode = 1; }    }

}
