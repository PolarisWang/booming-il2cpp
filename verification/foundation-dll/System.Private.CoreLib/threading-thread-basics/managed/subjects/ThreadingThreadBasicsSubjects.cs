// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/thread/basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Threading;

public static partial class ThreadingThreadBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()
    public static void Subject_0()
    {
        if ((int)(Thread.CurrentThread.ThreadState) != (int)(Thread.CurrentThread.ThreadState)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()
    public static void Subject_1()
    {
        if ((int)(Thread.CurrentThread.Priority) != (int)(Thread.CurrentThread.Priority)) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)
    public static void Subject_2()
    {
        Thread.CurrentThread.Priority = null!;
    }

    // [3] System.Private.CoreLib/System.Threading.Thread::get_IsBackground:System.Boolean()
    public static void Subject_3()
    {
        if (((Thread.CurrentThread.IsBackground) ? 1 : 0) != ((Thread.CurrentThread.IsBackground) ? 1 : 0)) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)
    public static void Subject_4()
    {
        Thread.CurrentThread.IsBackground = true;
    }

    // [5] System.Private.CoreLib/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()
    public static void Subject_5()
    {
        if (((Thread.CurrentThread.IsThreadPoolThread) ? 1 : 0) != ((Thread.CurrentThread.IsThreadPoolThread) ? 1 : 0)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Threading.Thread::Abort:System.Void()
    public static void Subject_6()
    {
        Thread.CurrentThread.Abort();
    }

    // [7] System.Private.CoreLib/System.Threading.Thread::Interrupt:System.Void()
    public static void Subject_7()
    {
        Thread.CurrentThread.Interrupt();
    }

    // [8] System.Private.CoreLib/System.Threading.Thread::ResetAbort:System.Boolean()
    public static void Subject_8()
    {
        if (((Thread.CurrentThread.ResetAbort()) ? 1 : 0) != ((Thread.CurrentThread.ResetAbort()) ? 1 : 0)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()
    public static void Subject_9()
    {
        if (((Thread.CurrentThread.Yield()) ? 1 : 0) != ((Thread.CurrentThread.Yield()) ? 1 : 0)) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void Subject_10()
    {
        // TODO: System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32) could not be auto-generated
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
        }
    }

}