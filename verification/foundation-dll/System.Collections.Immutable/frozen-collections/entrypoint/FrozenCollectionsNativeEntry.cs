// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/frozen-collections
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Frozen;

public static class FrozenCollectionsNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,int>:System.Collections.Frozen.FrozenDictionary<System.String,System.Int32>()
    static int Method0()
    {
        _ = FrozenDictionary.Create<string,int>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<Byte,Byte>:System.Collections.Frozen.FrozenDictionary<System.Byte,System.Byte>()
    static int Method1()
    {
        _ = FrozenDictionary.Create<Byte,Byte>();
        return 0;
    }

    // [2] System.Collections.Immutable/System.Collections.Frozen.FrozenDictionary::Create<string,string>:System.Collections.Frozen.FrozenDictionary<System.String,System.String>()
    static int Method2()
    {
        _ = FrozenDictionary.Create<string,string>();
        return 0;
    }

    // [3] System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>()
    static int Method3()
    {
        _ = FrozenSet.Create<Byte>();
        return 0;
    }

    // [4] System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte)
    static int Method4()
    {
        _ = FrozenSet.Create<Byte>((byte)42);
        return 0;
    }

    // [5] System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Byte>:System.Collections.Frozen.FrozenSet<System.Byte>(System.Byte,System.Byte)
    static int Method5()
    {
        _ = FrozenSet.Create<Byte>((byte)42, (byte)42);
        return 0;
    }

    // [6] System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>()
    static int Method6()
    {
        _ = FrozenSet.Create<Int32>();
        return 0;
    }

    // [7] System.Collections.Immutable/System.Collections.Frozen.FrozenSet::Create<Int32>:System.Collections.Frozen.FrozenSet<System.Int32>(System.Int32)
    static int Method7()
    {
        _ = FrozenSet.Create<Int32>(42);
        return 0;
    }

}