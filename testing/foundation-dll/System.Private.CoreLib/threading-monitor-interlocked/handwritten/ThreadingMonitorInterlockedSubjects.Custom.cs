// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ThreadingMonitorInterlockedSubjects
{
    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
    public static void CustomEntrySubject_1()
    {        try { Monitor.Exit(null!); } catch { _exitCode = 1; }    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
    public static void CustomEntrySubject_4()
    {        try { Monitor.Pulse(null!); } catch { _exitCode = 1; }    }

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
    public static void CustomEntrySubject_5()
    {        try { Monitor.PulseAll(null!); } catch { _exitCode = 1; }    }

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
    public static void CustomEntrySubject_6()
    {        try { _ = Monitor.Wait(null!); } catch { _exitCode = 1; }    }

}
