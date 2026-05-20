// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/monitor/interlocked
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

public static partial class ThreadingMonitorInterlockedSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)

    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)

    // [2] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
    public static void Subject_2()
    {
        if (((Monitor.TryEnter(42)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
    public static void Subject_3()
    {
        if (((Monitor.TryEnter(42, 42)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)

    // [7] System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)

    // [8] System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)

    // [9] System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)

    // [10] System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)

    // [11] System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)

    // [12] System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
    public static void Subject_12()
    {
        Interlocked.MemoryBarrier();
    }

    // [13] System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
    public static void Subject_13()
    {
    var refLocal_0 = 42;
        if (Volatile.Read(ref refLocal_0) != 42) _exitCode = 1;
    }

}