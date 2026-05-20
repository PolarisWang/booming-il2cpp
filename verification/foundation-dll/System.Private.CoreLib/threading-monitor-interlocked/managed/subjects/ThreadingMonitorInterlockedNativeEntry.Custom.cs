// Handwritten custom entry implementations for Monitor and Interlocked operations.
//
// Methods 2 (TryEnter), 3 (TryEnter with timeout), 12 (MemoryBarrier), and
// 13 (Volatile.Read) are auto-generated successfully. The remaining methods
// involve heap allocation (new object()), ref parameters, or Monitor.Wait/Pulse
// patterns that the auto-generator cannot emit.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System.Threading;

public static partial class ThreadingMonitorInterlockedNativeEntry
{
    public static int _exitCode;
    // [0] System.Threading.Monitor::Enter(System.Object)
    public static void CustomEntryMethod0()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Exit(lockObj);
    }

    // [1] System.Threading.Monitor::Exit(System.Object)
    public static void CustomEntryMethod1()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Exit(lockObj);
    }

    // [4] System.Threading.Monitor::Pulse(System.Object)
    public static void CustomEntryMethod4()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Pulse(lockObj);
        Monitor.Exit(lockObj);
    }

    // [5] System.Threading.Monitor::PulseAll(System.Object)
    public static void CustomEntryMethod5()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.PulseAll(lockObj);
        Monitor.Exit(lockObj);
    }

    // [6] System.Threading.Monitor::Wait(System.Object)
    public static void CustomEntryMethod6()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Pulse(lockObj);
        Monitor.Wait(lockObj, 1);
        Monitor.Exit(lockObj);
    }

    // [7] System.Threading.Interlocked::Increment(System.Int32&)
    public static void CustomEntryMethod7()
    {
        int val = 0;
        if (Interlocked.Increment(ref val) != 1) _exitCode = 1;
    }

    // [8] System.Threading.Interlocked::Decrement(System.Int32&)
    public static void CustomEntryMethod8()
    {
        int val = 1;
        if (Interlocked.Decrement(ref val) != 0) _exitCode = 1;
    }

    // [9] System.Threading.Interlocked::Exchange(System.Int32&, System.Int32)
    public static void CustomEntryMethod9()
    {
        int val = 42;
        if (Interlocked.Exchange(ref val, 99) != 42) _exitCode = 1;
        if (val != 99) _exitCode = 1;
    }

    // [10] System.Threading.Interlocked::CompareExchange(System.Int32&, System.Int32, System.Int32)
    public static void CustomEntryMethod10()
    {
        int val = 42;
        if (Interlocked.CompareExchange(ref val, 99, 42) != 42) _exitCode = 1;
        if (val != 99) _exitCode = 1;
    }

    // [11] System.Threading.Interlocked::Add(System.Int32&, System.Int32)
    public static void CustomEntryMethod11()
    {
        int val = 10;
        if (Interlocked.Add(ref val, 5) != 15) _exitCode = 1;
    }
}
