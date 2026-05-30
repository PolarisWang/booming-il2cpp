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

[CapabilityTest("family/System.Collections.Immutable/immutable/hash/set", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ImmutableHashSetTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable/hash/set

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
        // Purpose: Smoke — ImmutableHashSet.Create<Byte> with complex param(s)
            Assert.IsTrue(ImmutableHashSet.Create<Byte>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
        // Purpose: Smoke — ImmutableHashSet.Create<Byte> with complex param(s)
            var result = ImmutableHashSet.Create<Byte>((byte)42); Assert.AreEqual(1, result.Count);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Byte_System_Collections_Immutable_Immu()
    {
        // Purpose: Smoke — ImmutableHashSet.Create<Byte> with complex param(s)
            var result = ImmutableHashSet.Create<Byte>((byte)42, (byte)42); Assert.AreEqual(1, result.Count);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm()
    {
        // Purpose: Smoke — ImmutableHashSet.Create<Int32> with complex param(s)
            Assert.IsTrue(ImmutableHashSet.Create<Int32>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableHashSet_Create_Int32_System_Collections_Immutable_Imm()
    {
        // Purpose: Smoke — ImmutableHashSet.Create<Int32> with complex param(s)
            var result = ImmutableHashSet.Create<Int32>(42); Assert.AreEqual(1, result.Count);
    }
}
