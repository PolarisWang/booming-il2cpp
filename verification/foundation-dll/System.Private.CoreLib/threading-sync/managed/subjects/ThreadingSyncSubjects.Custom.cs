// Handwritten custom subject implementations for Thread Synchronization primitives.
//
// Subjects 2/3/4 (SemaphoreSlim instance methods), 6/7/8 (ManualResetEvent instance),
// 10/11/12 (AutoResetEvent instance), 14/15 (SpinLock instance), 16 (SpinWait instance)
// require proper instance creation before calling instance methods.
//
// This file is a partial class of the auto-generated ThreadingSyncSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Threading;

public static partial class ThreadingSyncSubjects
{
    private static SemaphoreSlim s_sem = new SemaphoreSlim(1, 1);
    private static ManualResetEvent s_mre = new ManualResetEvent(false);
    private static AutoResetEvent s_are = new AutoResetEvent(false);
    private static SpinLock s_slock = new SpinLock(true);

    // [2] System.Threading.SemaphoreSlim::Wait()
    public static void CustomEntrySubject_2()
    {
        s_sem.Wait();
        s_sem.Release();
    }

    // [3] System.Threading.SemaphoreSlim::Release()
    public static void CustomEntrySubject_3()
    {
        s_sem.Wait();
        int count = s_sem.Release();
        if (count < 0) _exitCode = 1;
    }

    // [4] System.Threading.SemaphoreSlim::get_CurrentCount()
    public static void CustomEntrySubject_4()
    {
        int c = s_sem.CurrentCount;
        if (c < 0) _exitCode = 1;
    }

    // [6] System.Threading.ManualResetEvent::Set()
    public static void CustomEntrySubject_6()
    {
        s_mre.Reset();
        if (!s_mre.Set()) _exitCode = 1;
    }

    // [7] System.Threading.ManualResetEvent::Reset()
    public static void CustomEntrySubject_7()
    {
        s_mre.Set();
        if (!s_mre.Reset()) _exitCode = 1;
    }

    // [8] System.Threading.ManualResetEvent::WaitOne()
    public static void CustomEntrySubject_8()
    {
        s_mre.Set();
        if (!s_mre.WaitOne(1)) _exitCode = 1;
    }

    // [10] System.Threading.AutoResetEvent::Set()
    public static void CustomEntrySubject_10()
    {
        if (!s_are.Set()) _exitCode = 1;
    }

    // [11] System.Threading.AutoResetEvent::Reset()
    public static void CustomEntrySubject_11()
    {
        s_are.Set();
        if (!s_are.Reset()) _exitCode = 1;
    }

    // [12] System.Threading.AutoResetEvent::WaitOne()
    public static void CustomEntrySubject_12()
    {
        s_are.Set();
        if (!s_are.WaitOne(1)) _exitCode = 1;
    }

    // [14] System.Threading.SpinLock::Enter(Boolean&)
    public static void CustomEntrySubject_14()
    {
        bool lockTaken = false;
        s_slock.Enter(ref lockTaken);
        if (!lockTaken) _exitCode = 1;
        s_slock.Exit();
    }

    // [15] System.Threading.SpinLock::Exit()
    public static void CustomEntrySubject_15()
    {
        bool lockTaken = false;
        var sl = new SpinLock(true);
        sl.Enter(ref lockTaken);
        sl.Exit();
    }

    // [16] System.Threading.SpinWait::SpinOnce()
    public static void CustomEntrySubject_16()
    {
        var sw = new SpinWait();
        sw.SpinOnce();
    }
}
