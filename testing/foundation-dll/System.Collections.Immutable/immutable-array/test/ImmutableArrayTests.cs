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

[CapabilityTest("family/System.Collections.Immutable/immutable/array", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ImmutableArrayTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable/array

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableArray.Create<Byte> with complex param(s)
            Assert.IsTrue(ImmutableArray.Create<Byte>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableArray.Create<Byte> with complex param(s)
            var result = ImmutableArray.Create<Byte>((byte)42); Assert.IsTrue(!result.IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Byte_System_Collections_Immutable_Immuta()
    {
        // Purpose: Smoke — ImmutableArray.Create<Byte> with complex param(s)
            var result = ImmutableArray.Create<Byte>((byte)42, (byte)42); Assert.AreEqual(2, result.Length);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
        // Purpose: Smoke — ImmutableArray.Create<Int32> with complex param(s)
            Assert.IsTrue(ImmutableArray.Create<Int32>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_Int32_System_Collections_Immutable_Immut()
    {
        // Purpose: Smoke — ImmutableArray.Create<Int32> with complex param(s)
            var result = ImmutableArray.Create<Int32>(42); Assert.IsTrue(!result.IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Create_String_System_Collections_Immutable_Immu()
    {
        // Purpose: Smoke — ImmutableArray.Create<String> with complex param(s)
            var result = ImmutableArray.Create<String>("hello"); Assert.AreEqual("hello", result[0]);
    }
}
