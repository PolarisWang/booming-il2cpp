// Handwritten custom patch implementations for Thread Synchronization primitives.
//
// Patch variant uses CustomEntryMethod{N} naming (not CustomEntrySubject_{N}).
// Pipeline includes this file automatically when present.

using System.Threading;

public static partial class ThreadingSyncPatchEntry
{
    private static SemaphoreSlim s_sem = new SemaphoreSlim(1, 1);
    private static ManualResetEvent s_mre = new ManualResetEvent(false);
    private static AutoResetEvent s_are = new AutoResetEvent(false);
    private static SpinLock s_slock = new SpinLock(true);

    // [2] System.Threading.SemaphoreSlim::Wait()
    public static void CustomEntryMethod2()
    {
        s_sem.Wait();
        s_sem.Release();
    }

    // [3] System.Threading.SemaphoreSlim::Release()
    public static void CustomEntryMethod3()
    {
        s_sem.Wait();
        int count = s_sem.Release();
        if (count < 0) _exitCode = 1;
    }

    // [4] System.Threading.SemaphoreSlim::get_CurrentCount()
    public static void CustomEntryMethod4()
    {
        int c = s_sem.CurrentCount;
        if (c < 0) _exitCode = 1;
    }

    // [6] System.Threading.ManualResetEvent::Set()
    public static void CustomEntryMethod6()
    {
        s_mre.Reset();
        if (!s_mre.Set()) _exitCode = 1;
    }

    // [7] System.Threading.ManualResetEvent::Reset()
    public static void CustomEntryMethod7()
    {
        s_mre.Set();
        if (!s_mre.Reset()) _exitCode = 1;
    }

    // [8] System.Threading.ManualResetEvent::WaitOne()
    public static void CustomEntryMethod8()
    {
        s_mre.Set();
        if (!s_mre.WaitOne(1)) _exitCode = 1;
    }

    // [10] System.Threading.AutoResetEvent::Set()
    public static void CustomEntryMethod10()
    {
        if (!s_are.Set()) _exitCode = 1;
    }

    // [11] System.Threading.AutoResetEvent::Reset()
    public static void CustomEntryMethod11()
    {
        s_are.Set();
        if (!s_are.Reset()) _exitCode = 1;
    }

    // [12] System.Threading.AutoResetEvent::WaitOne()
    public static void CustomEntryMethod12()
    {
        s_are.Set();
        if (!s_are.WaitOne(1)) _exitCode = 1;
    }

    // [14] System.Threading.SpinLock::Enter(Boolean&)
    public static void CustomEntryMethod14()
    {
        bool lockTaken = false;
        s_slock.Enter(ref lockTaken);
        if (!lockTaken) _exitCode = 1;
        s_slock.Exit();
    }

    // [15] System.Threading.SpinLock::Exit()
    public static void CustomEntryMethod15()
    {
        bool lockTaken = false;
        var sl = new SpinLock(true);
        sl.Enter(ref lockTaken);
        sl.Exit();
    }

    // [16] System.Threading.SpinWait::SpinOnce()
    public static void CustomEntryMethod16()
    {
        var sw = new SpinWait();
        sw.SpinOnce();
    }
}
