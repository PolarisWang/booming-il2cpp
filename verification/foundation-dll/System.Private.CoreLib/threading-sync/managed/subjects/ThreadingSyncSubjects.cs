// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/sync
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

public static partial class ThreadingSyncSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32)
    public static void Subject_0()
    {
        new SemaphoreSlim(42);
    }

    // [1] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)
    public static void Subject_1()
    {
        new SemaphoreSlim(42, 42);
    }

    // [2] System.Private.CoreLib/System.Threading.SemaphoreSlim::Wait:System.Void()

    // [3] System.Private.CoreLib/System.Threading.SemaphoreSlim::Release:System.Int32()

    // [4] System.Private.CoreLib/System.Threading.SemaphoreSlim::get_CurrentCount:System.Int32()

    // [5] System.Private.CoreLib/System.Threading.ManualResetEvent::.ctor:System.Void(System.Boolean)
    public static void Subject_5()
    {
        new ManualResetEvent(true);
    }

    // [6] System.Private.CoreLib/System.Threading.ManualResetEvent::Set:System.Boolean()

    // [7] System.Private.CoreLib/System.Threading.ManualResetEvent::Reset:System.Boolean()

    // [8] System.Private.CoreLib/System.Threading.ManualResetEvent::WaitOne:System.Boolean()

    // [9] System.Private.CoreLib/System.Threading.AutoResetEvent::.ctor:System.Void(System.Boolean)
    public static void Subject_9()
    {
        new AutoResetEvent(true);
    }

    // [10] System.Private.CoreLib/System.Threading.AutoResetEvent::Set:System.Boolean()

    // [11] System.Private.CoreLib/System.Threading.AutoResetEvent::Reset:System.Boolean()

    // [12] System.Private.CoreLib/System.Threading.AutoResetEvent::WaitOne:System.Boolean()

    // [13] System.Private.CoreLib/System.Threading.SpinLock::.ctor:System.Void(System.Boolean)
    public static void Subject_13()
    {
        new SpinLock(true);
    }

    // [14] System.Private.CoreLib/System.Threading.SpinLock::Enter:System.Void(System.Boolean&)

    // [15] System.Private.CoreLib/System.Threading.SpinLock::Exit:System.Void()

    // [16] System.Private.CoreLib/System.Threading.SpinWait::SpinOnce:System.Void()

}