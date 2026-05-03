// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-sorted-dictionary
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableSortedDictionaryNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<string,int>:System.Collections.Immutable.ImmutableSortedDictionary<System.String,System.Int32>()
    static int Method0()
    {
        _ = ImmutableSortedDictionary.Create<string,int>();
        return 0;
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedDictionary::Create<Byte,Byte>:System.Collections.Immutable.ImmutableSortedDictionary<System.Byte,System.Byte>()
    static int Method1()
    {
        _ = ImmutableSortedDictionary.Create<Byte,Byte>();
        return 0;
    }

}