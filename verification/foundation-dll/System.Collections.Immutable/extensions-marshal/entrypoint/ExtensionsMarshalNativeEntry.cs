// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/extensions-marshal
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ExtensionsMarshalNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
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

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()
    static int Method2()
    {
        _ = ImmutableArray.Create<Int32>();
        return 0;
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)
    static int Method3()
    {
        _ = ImmutableArray.Create<Int32>(42);
        return 0;
    }

}