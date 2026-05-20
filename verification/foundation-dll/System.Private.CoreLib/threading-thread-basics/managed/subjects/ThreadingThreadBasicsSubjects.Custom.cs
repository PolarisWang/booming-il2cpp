// Handwritten custom subject implementations for Thread basics (State, Priority, Abort, Interrupt).
//
// All 11 Thread methods in this family are instance methods on System.Threading.Thread,
// which the auto-generator cannot safely produce (no Thread instance in a static context
// for get_ThreadState, Abort, Interrupt, etc.).
//
// Methods placed here by customEntryIndices in capability-family-contract.json:
// indices [0..10] (= all).
//
// This file is a partial class of the auto-generated ThreadingThreadBasicsSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Threading;

public static partial class ThreadingThreadBasicsSubjects
{
    // [0] System.Threading.Thread::get_ThreadState
    public static void CustomEntrySubject_0()
    {
        var state = Thread.CurrentThread.ThreadState;
    }

    // [1] System.Threading.Thread::get_Priority
    public static void CustomEntrySubject_1()
    {
        var pri = Thread.CurrentThread.Priority;
    }

    // [2] System.Threading.Thread::set_Priority(System.Threading.ThreadPriority)
    public static void CustomEntrySubject_2()
    {
        Thread.CurrentThread.Priority = ThreadPriority.Normal;
    }

    // [3] System.Threading.Thread::get_IsBackground
    public static void CustomEntrySubject_3()
    {
        var bg = Thread.CurrentThread.IsBackground;
    }

    // [4] System.Threading.Thread::set_IsBackground(System.Boolean)
    public static void CustomEntrySubject_4()
    {
        // Setting IsBackground on the current thread only modifies a flag,
        // it does not throw (even for the main thread).
        Thread.CurrentThread.IsBackground = false;
    }

    // [5] System.Threading.Thread::get_IsThreadPoolThread
    public static void CustomEntrySubject_5()
    {
        var tp = Thread.CurrentThread.IsThreadPoolThread;
    }

    // [6] System.Threading.Thread::Abort
    public static void CustomEntrySubject_6()
    {
        // NOTE: Obsolete since .NET 6, throws PlatformNotSupportedException.
        // Wrap in try/catch so this method is a no-op in modern runtimes.
        try { Thread.CurrentThread.Abort(); }
        catch (PlatformNotSupportedException) { }
    }

    // [7] System.Threading.Thread::Interrupt
    public static void CustomEntrySubject_7()
    {
        // Calling Interrupt on the current thread sets a pending interrupt that
        // would cause the next Wait/Sleep/Join to throw ThreadInterruptedException.
        // Call it then immediately consume the pending interrupt to avoid affecting
        // subsequent methods (especially Subject_10 = Sleep) in the fact loop.
        Thread.CurrentThread.Interrupt();
        try { Thread.Sleep(0); }
        catch (ThreadInterruptedException) { }
    }

    // [8] System.Threading.Thread::ResetAbort
    public static void CustomEntrySubject_8()
    {
        // NOTE: Obsolete since .NET 6, throws PlatformNotSupportedException.
        try { Thread.ResetAbort(); }
        catch (PlatformNotSupportedException) { }
    }

    // [9] System.Threading.Thread::Yield
    public static void CustomEntrySubject_9()
    {
        Thread.Yield();
    }

    // [10] System.Threading.Thread::Sleep(System.Int32)
    public static void CustomEntrySubject_10()
    {
        Thread.Sleep(1);
    }
}
