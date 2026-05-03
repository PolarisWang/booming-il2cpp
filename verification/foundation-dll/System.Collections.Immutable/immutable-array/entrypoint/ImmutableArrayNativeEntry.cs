// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-array
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableArrayNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()
    static int Method0()
    {
        _ = ImmutableArray.Create<Byte>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)
    static int Method1()
    {
        _ = ImmutableArray.Create<Byte>((byte)42);
        return 0;
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte,System.Byte)
    static int Method2()
    {
        _ = ImmutableArray.Create<Byte>((byte)42, (byte)42);
        return 0;
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()
    static int Method3()
    {
        _ = ImmutableArray.Create<Int32>();
        return 0;
    }

    // [4] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)
    static int Method4()
    {
        _ = ImmutableArray.Create<Int32>(42);
        return 0;
    }

    // [5] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<String>:System.Collections.Immutable.ImmutableArray<System.String>(System.String)
    static int Method5()
    {
        _ = ImmutableArray.Create<String>("hello");
        return 0;
    }

}