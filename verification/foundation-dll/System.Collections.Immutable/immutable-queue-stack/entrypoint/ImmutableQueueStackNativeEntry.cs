// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-queue-stack
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableQueueStackNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>()
    static int Method0()
    {
        _ = ImmutableQueue.Create<Byte>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte)
    static int Method1()
    {
        _ = ImmutableQueue.Create<Byte>((byte)42);
        return 0;
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Byte>:System.Collections.Immutable.ImmutableQueue<System.Byte>(System.Byte,System.Byte)
    static int Method2()
    {
        _ = ImmutableQueue.Create<Byte>((byte)42, (byte)42);
        return 0;
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>()
    static int Method3()
    {
        _ = ImmutableQueue.Create<Int32>();
        return 0;
    }

    // [4] System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Create<Int32>:System.Collections.Immutable.ImmutableQueue<System.Int32>(System.Int32)
    static int Method4()
    {
        _ = ImmutableQueue.Create<Int32>(42);
        return 0;
    }

    // [5] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>()
    static int Method5()
    {
        _ = ImmutableStack.Create<Byte>();
        return 0;
    }

    // [6] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte)
    static int Method6()
    {
        _ = ImmutableStack.Create<Byte>((byte)42);
        return 0;
    }

    // [7] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Byte>:System.Collections.Immutable.ImmutableStack<System.Byte>(System.Byte,System.Byte)
    static int Method7()
    {
        _ = ImmutableStack.Create<Byte>((byte)42, (byte)42);
        return 0;
    }

    // [8] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>()
    static int Method8()
    {
        _ = ImmutableStack.Create<Int32>();
        return 0;
    }

    // [9] System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Create<Int32>:System.Collections.Immutable.ImmutableStack<System.Int32>(System.Int32)
    static int Method9()
    {
        _ = ImmutableStack.Create<Int32>(42);
        return 0;
    }

}