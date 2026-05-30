// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class PinvokeStressSubjects
{
    // [0] System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()
    public static void CustomEntrySubject_0()
    {        try { _ = Environment.TickCount; } catch { _exitCode = 1; }    }

    // [2] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void CustomEntrySubject_2()
    {        try { _ = GC.GetTotalMemory(true); } catch { _exitCode = 1; }    }

    // [3] System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void CustomEntrySubject_3()
    {        try { _ = Marshal.GetLastPInvokeError(); } catch { _exitCode = 1; }    }

}
