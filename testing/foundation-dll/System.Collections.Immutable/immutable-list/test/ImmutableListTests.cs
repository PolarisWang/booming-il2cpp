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

[CapabilityTest("family/System.Collections.Immutable/immutable/list", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ImmutableListTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable/list

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab()
    {
        // Purpose: Smoke — ImmutableList.Create<Byte> with complex param(s)
            _ = ImmutableList.Create<Byte>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab()
    {
        // Purpose: Smoke — ImmutableList.Create<Byte> with complex param(s)
            _ = ImmutableList.Create<Byte>((byte)42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Byte_System_Collections_Immutable_Immutab()
    {
        // Purpose: Smoke — ImmutableList.Create<Byte> with complex param(s)
            _ = ImmutableList.Create<Byte>((byte)42, (byte)42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableList.Create<Int32> with complex param(s)
            _ = ImmutableList.Create<Int32>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_Int32_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableList.Create<Int32> with complex param(s)
            _ = ImmutableList.Create<Int32>(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableList_Create_String_System_Collections_Immutable_Immut()
    {
        // Purpose: Smoke — ImmutableList.Create<String> with complex param(s)
            _ = ImmutableList.Create<String>("hello");
    }
}
