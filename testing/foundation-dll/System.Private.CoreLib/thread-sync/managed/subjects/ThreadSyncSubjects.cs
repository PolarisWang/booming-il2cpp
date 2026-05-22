// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/thread/sync
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

public static partial class ThreadSyncSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void Subject_0()
    {
        try { Thread.Sleep(42); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()
    public static void Subject_1()
    {
        try { _ = Thread.Yield(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Threading.CancellationTokenSource::Cancel:System.Void()
    public static void Subject_2()
    {
        try { new CancellationTokenSource().Cancel(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Threading.CancellationToken::get_IsCancellationRequested:System.Boolean()
    public static void Subject_3()
    {
        try { _ = default(CancellationToken).IsCancellationRequested; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Threading.CancellationToken::Register:System.Threading.CancellationTokenRegistration(System.Action)
    public static void Subject_4()
    {
        try { _ = default(CancellationToken).Register(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback)
    public static void Subject_5()
    {
        try { _ = ThreadPool.QueueUserWorkItem(new WaitCallback(static _ => {})); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback,System.Object)
    public static void Subject_6()
    {
        try { _ = ThreadPool.QueueUserWorkItem(new WaitCallback(static _ => {}), 42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Threading.WaitCallback::.ctor:System.Void(System.Object,System.IntPtr)
    public static void Subject_7()
    {
        try { new WaitCallback(static _ => { }).GetInvocationList(); }
        catch { _exitCode = 1; }
    }

}