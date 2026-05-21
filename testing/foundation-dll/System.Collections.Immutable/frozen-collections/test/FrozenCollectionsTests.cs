using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Collections.Immutable/frozen/collections", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class FrozenCollectionsTests
{
    // AI-generated tests for family/System.Collections.Immutable/frozen/collections

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_int_System_Collections_Frozen_Froz()
    {
        // Purpose: Smoke — FrozenDictionary.Create<string,int> with complex param(s)
            _ = FrozenDictionary.Create<string,int>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_Byte_Byte_System_Collections_Frozen_Froze()
    {
        // Purpose: Smoke — FrozenDictionary.Create<Byte,Byte> with complex param(s)
            _ = FrozenDictionary.Create<Byte,Byte>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Create_string_string_System_Collections_Frozen_F()
    {
        // Purpose: Smoke — FrozenDictionary.Create<string,string> with complex param(s)
            _ = FrozenDictionary.Create<string,string>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
        // Purpose: Smoke — FrozenSet.Create<Byte> with complex param(s)
            _ = FrozenSet.Create<Byte>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
        // Purpose: Smoke — FrozenSet.Create<Byte> with complex param(s)
            _ = FrozenSet.Create<Byte>((byte)42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Byte_System_Collections_Frozen_FrozenSet_System_()
    {
        // Purpose: Smoke — FrozenSet.Create<Byte> with complex param(s)
            _ = FrozenSet.Create<Byte>((byte)42, (byte)42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System()
    {
        // Purpose: Smoke — FrozenSet.Create<Int32> with complex param(s)
            _ = FrozenSet.Create<Int32>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Create_Int32_System_Collections_Frozen_FrozenSet_System()
    {
        // Purpose: Smoke — FrozenSet.Create<Int32> with complex param(s)
            _ = FrozenSet.Create<Int32>(42);
    }
}
