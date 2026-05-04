// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/monitor/interlocked
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Threading;

public static class ThreadingMonitorInterlockedNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object) could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
    static int Method2()
    {
        return ((Monitor.TryEnter(42)) ? 1 : 0);
    }

    // [3] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
    static int Method3()
    {
        return ((Monitor.TryEnter(42, 42)) ? 1 : 0);
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
    static int Method4()
    {
        // TODO: System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object) could not be auto-generated
        return 0;
    }

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
    static int Method5()
    {
        // TODO: System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object) could not be auto-generated
        return 0;
    }

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object) could not be auto-generated
        return 0;
    }

    // [7] System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&) could not be auto-generated
        return 0;
    }

    // [8] System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)
    static int Method8()
    {
        // TODO: System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&) could not be auto-generated
        return 0;
    }

    // [9] System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32) could not be auto-generated
        return 0;
    }

    // [10] System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)
    static int Method10()
    {
        // TODO: System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32) could not be auto-generated
        return 0;
    }

    // [11] System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)
    static int Method11()
    {
        // TODO: System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32) could not be auto-generated
        return 0;
    }

    // [12] System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
    static int Method12()
    {
        Interlocked.MemoryBarrier();
        return (int)0;
    }

    // [13] System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
    static int Method13()
    {
    var refLocal_0 = 42;
        return Volatile.Read(ref refLocal_0);
    }

}