// Handwritten custom subject implementations for Monitor and Interlocked operations.
//
// Methods placed here by customEntryIndices in capability-family-contract.json:
// Subject_2 (TryEnter), Subject_3 (TryEnter with timeout), Subject_12 (MemoryBarrier),
// and Subject_13 (Volatile.Read) are auto-generated successfully.
//
// This file is a partial class of the auto-generated ThreadingMonitorInterlockedSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Threading;

public static partial class ThreadingMonitorInterlockedSubjects
{
    // [0] System.Threading.Monitor::Enter(System.Object)
    public static void CustomEntrySubject_0()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Exit(lockObj);
    }

    // [1] System.Threading.Monitor::Exit(System.Object)
    public static void CustomEntrySubject_1()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Exit(lockObj);
    }

    // [4] System.Threading.Monitor::Pulse(System.Object)
    public static void CustomEntrySubject_4()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Pulse(lockObj);
        Monitor.Exit(lockObj);
    }

    // [5] System.Threading.Monitor::PulseAll(System.Object)
    public static void CustomEntrySubject_5()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.PulseAll(lockObj);
        Monitor.Exit(lockObj);
    }

    // [6] System.Threading.Monitor::Wait(System.Object)
    public static void CustomEntrySubject_6()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Pulse(lockObj);
        Monitor.Wait(lockObj, 1);
        Monitor.Exit(lockObj);
    }

    // [7] System.Threading.Interlocked::Increment(System.Int32&)
    public static void CustomEntrySubject_7()
    {
        int val = 0;
        if (Interlocked.Increment(ref val) != 1) _exitCode = 1;
    }

    // [8] System.Threading.Interlocked::Decrement(System.Int32&)
    public static void CustomEntrySubject_8()
    {
        int val = 1;
        if (Interlocked.Decrement(ref val) != 0) _exitCode = 1;
    }

    // [9] System.Threading.Interlocked::Exchange(System.Int32&, System.Int32)
    public static void CustomEntrySubject_9()
    {
        int val = 42;
        if (Interlocked.Exchange(ref val, 99) != 42) _exitCode = 1;
        if (val != 99) _exitCode = 1;
    }

    // [10] System.Threading.Interlocked::CompareExchange(System.Int32&, System.Int32, System.Int32)
    public static void CustomEntrySubject_10()
    {
        int val = 42;
        if (Interlocked.CompareExchange(ref val, 99, 42) != 42) _exitCode = 1;
        if (val != 99) _exitCode = 1;
    }

    // [11] System.Threading.Interlocked::Add(System.Int32&, System.Int32)
    public static void CustomEntrySubject_11()
    {
        int val = 10;
        if (Interlocked.Add(ref val, 5) != 15) _exitCode = 1;
    }
}
