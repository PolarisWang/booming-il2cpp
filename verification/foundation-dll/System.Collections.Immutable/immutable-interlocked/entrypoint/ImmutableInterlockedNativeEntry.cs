// Auto-generated native-AOT entry point
// Family: family/System.Collections.Immutable/immutable-interlocked
// Assembly: System.Collections.Immutable
// Variant: benchmark

using System;
using System.Collections.Immutable;

public static class ImmutableInterlockedNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            default: return -1;
        }
    }

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})
    static int Method0()
    {
    var refLocal_0 = "hello";
        return (ImmutableInterlocked.Update<string>(ref refLocal_0, null!) ? 1 : 0);
    }

}