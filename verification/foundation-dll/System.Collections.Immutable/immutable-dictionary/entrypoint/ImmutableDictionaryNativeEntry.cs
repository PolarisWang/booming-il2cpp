// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-dictionary
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableDictionaryNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,int>:System.Collections.Immutable.ImmutableDictionary<System.String,System.Int32>()
    static int Method0()
    {
        _ = ImmutableDictionary.Create<string,int>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableDictionary<System.Byte,System.Byte>()
    static int Method1()
    {
        _ = ImmutableDictionary.Create<Byte,Byte>();
        return 0;
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableDictionary::Create<string,string>:System.Collections.Immutable.ImmutableDictionary<System.String,System.String>()
    static int Method2()
    {
        _ = ImmutableDictionary.Create<string,string>();
        return 0;
    }

}