// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-hash-set
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableHashSetNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>()
    static int Method0()
    {
        _ = ImmutableHashSet.Create<Byte>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte)
    static int Method1()
    {
        _ = ImmutableHashSet.Create<Byte>((byte)42);
        return 0;
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Byte>:System.Collections.Immutable.ImmutableHashSet<System.Byte>(System.Byte,System.Byte)
    static int Method2()
    {
        _ = ImmutableHashSet.Create<Byte>((byte)42, (byte)42);
        return 0;
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>()
    static int Method3()
    {
        _ = ImmutableHashSet.Create<Int32>();
        return 0;
    }

    // [4] System.Collections.Immutable/System.Collections.Immutable.ImmutableHashSet::Create<Int32>:System.Collections.Immutable.ImmutableHashSet<System.Int32>(System.Int32)
    static int Method4()
    {
        _ = ImmutableHashSet.Create<Int32>(42);
        return 0;
    }

}