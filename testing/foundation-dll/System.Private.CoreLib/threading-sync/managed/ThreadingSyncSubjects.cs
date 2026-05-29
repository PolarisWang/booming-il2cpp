// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/sync
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ThreadingSyncSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32)
    public static void Subject_0()
    {
        try { new SemaphoreSlim(42); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)
    public static void Subject_1()
    {
        try { new SemaphoreSlim(42, 42); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Threading.SemaphoreSlim::Wait:System.Void()
    public static void CustomEntrySubject_2()
    {
    }

    // [3] System.Private.CoreLib/System.Threading.SemaphoreSlim::Release:System.Int32()
    public static void CustomEntrySubject_3()
    {
    }

    // [4] System.Private.CoreLib/System.Threading.SemaphoreSlim::get_CurrentCount:System.Int32()
    public static void CustomEntrySubject_4()
    {
    }

    // [5] System.Private.CoreLib/System.Threading.ManualResetEvent::.ctor:System.Void(System.Boolean)
    public static void Subject_5()
    {
        try { new ManualResetEvent(true); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Threading.ManualResetEvent::Set:System.Boolean()
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Private.CoreLib/System.Threading.ManualResetEvent::Reset:System.Boolean()
    public static void CustomEntrySubject_7()
    {
    }

    // [8] System.Private.CoreLib/System.Threading.ManualResetEvent::WaitOne:System.Boolean()
    public static void CustomEntrySubject_8()
    {
    }

    // [9] System.Private.CoreLib/System.Threading.AutoResetEvent::.ctor:System.Void(System.Boolean)
    public static void Subject_9()
    {
        try { new AutoResetEvent(true); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Threading.AutoResetEvent::Set:System.Boolean()
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.Private.CoreLib/System.Threading.AutoResetEvent::Reset:System.Boolean()
    public static void CustomEntrySubject_11()
    {
    }

    // [12] System.Private.CoreLib/System.Threading.AutoResetEvent::WaitOne:System.Boolean()
    public static void CustomEntrySubject_12()
    {
    }

    // [13] System.Private.CoreLib/System.Threading.SpinLock::.ctor:System.Void(System.Boolean)
    public static void Subject_13()
    {
        try { new SpinLock(true); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Threading.SpinLock::Enter:System.Void(System.Boolean&)
    public static void CustomEntrySubject_14()
    {
    }

    // [15] System.Private.CoreLib/System.Threading.SpinLock::Exit:System.Void()
    public static void CustomEntrySubject_15()
    {
    }

    // [16] System.Private.CoreLib/System.Threading.SpinWait::SpinOnce:System.Void()
    public static void CustomEntrySubject_16()
    {
    }

}