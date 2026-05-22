// Handwritten custom entry implementations for Thread basics (State, Priority, Abort, Interrupt).
//
// All 11 Thread methods are instance methods that the auto-generator cannot
// safely produce in a static context.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System;
using System.Threading;

public static partial class ThreadingThreadBasicsPatchEntry
{
    // _exitCode is provided by the auto-generated partial class.

    // [0] System.Threading.Thread::get_ThreadState
    public static void CustomEntryMethod0()
    {
        var state = Thread.CurrentThread.ThreadState;
    }

    // [1] System.Threading.Thread::get_Priority
    public static void CustomEntryMethod1()
    {
        var pri = Thread.CurrentThread.Priority;
    }

    // [2] System.Threading.Thread::set_Priority(System.Threading.ThreadPriority)
    public static void CustomEntryMethod2()
    {
        Thread.CurrentThread.Priority = ThreadPriority.Normal;
    }

    // [3] System.Threading.Thread::get_IsBackground
    public static void CustomEntryMethod3()
    {
        var bg = Thread.CurrentThread.IsBackground;
    }

    // [4] System.Threading.Thread::set_IsBackground(System.Boolean)
    public static void CustomEntryMethod4()
    {
        Thread.CurrentThread.IsBackground = false;
    }

    // [5] System.Threading.Thread::get_IsThreadPoolThread
    public static void CustomEntryMethod5()
    {
        var tp = Thread.CurrentThread.IsThreadPoolThread;
    }

    // [6] System.Threading.Thread::Abort
    public static void CustomEntryMethod6()
    {
        try { Thread.CurrentThread.Abort(); }
        catch (PlatformNotSupportedException) { }
    }

    // [7] System.Threading.Thread::Interrupt
    public static void CustomEntryMethod7()
    {
        Thread.CurrentThread.Interrupt();
        try { Thread.Sleep(0); }
        catch (ThreadInterruptedException) { }
    }

    // [8] System.Threading.Thread::ResetAbort
    public static void CustomEntryMethod8()
    {
        try { Thread.ResetAbort(); }
        catch (PlatformNotSupportedException) { }
    }

    // [9] System.Threading.Thread::Yield
    public static void CustomEntryMethod9()
    {
        Thread.Yield();
    }

    // [10] System.Threading.Thread::Sleep(System.Int32)
    public static void CustomEntryMethod10()
    {
        Thread.Sleep(1);
    }
}
