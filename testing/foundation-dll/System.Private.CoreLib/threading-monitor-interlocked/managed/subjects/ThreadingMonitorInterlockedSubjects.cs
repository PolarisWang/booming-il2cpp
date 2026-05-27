// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/monitor/interlocked
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

public static partial class ThreadingMonitorInterlockedSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)
    public static void CustomEntrySubject_0()
    {
    }

    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
    public static void Subject_2()
    {
        try { var _ = ((Monitor.TryEnter(null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
    public static void Subject_3()
    {
        try { var _ = ((Monitor.TryEnter(null!, 42)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
    public static void CustomEntrySubject_4()
    {
    }

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
    public static void CustomEntrySubject_5()
    {
    }

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)
    public static void CustomEntrySubject_7()
    {
    }

    // [8] System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)
    public static void CustomEntrySubject_8()
    {
    }

    // [9] System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)
    public static void CustomEntrySubject_9()
    {
    }

    // [10] System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)
    public static void CustomEntrySubject_11()
    {
    }

    // [12] System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
    public static void Subject_12()
    {
        try { Interlocked.MemoryBarrier(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
    public static void Subject_13()
    {
        try { var _ = ((System.Func<int>)(() => { int __v = 42; return System.Threading.Volatile.Read(ref __v); }))(); }
        catch { _exitCode = 1; }
    }

}