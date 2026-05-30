// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ThreadingMonitorInterlockedSubjects
{
    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        Monitor.Exit(null!);
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        Monitor.Pulse(null!);
    }

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        Monitor.PulseAll(null!);
    }

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        _ = Monitor.Wait(null!);
    }

}
