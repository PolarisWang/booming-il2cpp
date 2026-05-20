// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/thread/basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Threading;

public static partial class ThreadingThreadBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()
    public static void Subject_0()
    {
        if ((int)(Thread.CurrentThread.ThreadState) != 0) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()
    public static void Subject_1()
    {
        if ((int)(Thread.CurrentThread.Priority) != 2) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)
    public static void Subject_2()
    {
        Thread.CurrentThread.Priority = System.Threading.ThreadPriority.Normal;
    }

    // [3] System.Private.CoreLib/System.Threading.Thread::get_IsBackground:System.Boolean()
    public static void Subject_3()
    {
        if (((Thread.CurrentThread.IsBackground) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)
    public static void Subject_4()
    {
        Thread.CurrentThread.IsBackground = true;
    }

    // [5] System.Private.CoreLib/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()
    public static void Subject_5()
    {
        if (((Thread.CurrentThread.IsThreadPoolThread) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Threading.Thread::Abort:System.Void()
    public static void Subject_6()
    {
        try { Thread.CurrentThread.Abort(); _exitCode = 1; }
        catch (PlatformNotSupportedException) { }
    }

    // [7] System.Private.CoreLib/System.Threading.Thread::Interrupt:System.Void()
    public static void Subject_7()
    {
        Thread.CurrentThread.Interrupt();
    }

    // [8] System.Private.CoreLib/System.Threading.Thread::ResetAbort:System.Boolean()
    public static void Subject_8()
    {
        // TODO: System.Private.CoreLib/System.Threading.Thread::ResetAbort:System.Boolean() could not be auto-generated
    }

    // [9] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()
    public static void Subject_9()
    {
        if (((Thread.Yield()) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void Subject_10()
    {
        try { Thread.Sleep(42); _exitCode = 1; }
        catch (ThreadInterruptedException) { }
    }

}