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

[CapabilityTest("family/System.Collections.Immutable/extensions/marshal", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ExtensionsMarshalTests
{
    // AI-generated tests for family/System.Collections.Immutable/extensions/marshal

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableArray.Create<Byte> with complex param(s)
            _ = ImmutableArray.Create<Byte>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableArray.Create<Byte> with complex param(s)
            _ = ImmutableArray.Create<Byte>((byte)42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
        // Purpose: Smoke — ImmutableArray.Create<Int32> with complex param(s)
            _ = ImmutableArray.Create<Int32>();
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
        // Purpose: Smoke — ImmutableArray.Create<Int32> with complex param(s)
            _ = ImmutableArray.Create<Int32>(42);
    }
}
