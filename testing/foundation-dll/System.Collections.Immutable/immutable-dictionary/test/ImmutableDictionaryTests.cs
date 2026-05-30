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

[CapabilityTest("family/System.Collections.Immutable/immutable/dictionary", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ImmutableDictionaryTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable/dictionary

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_int_System_Collections_Immut()
    {
        // Purpose: Smoke — ImmutableDictionary.Create<string,int> with complex param(s)
            Assert.IsTrue(ImmutableDictionary.Create<string,int>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_Byte_Byte_System_Collections_Immuta()
    {
        // Purpose: Smoke — ImmutableDictionary.Create<Byte,Byte> with complex param(s)
            Assert.IsTrue(ImmutableDictionary.Create<Byte,Byte>().IsEmpty);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableDictionary_Create_string_string_System_Collections_Im()
    {
        // Purpose: Smoke — ImmutableDictionary.Create<string,string> with complex param(s)
            Assert.IsTrue(ImmutableDictionary.Create<string,string>().IsEmpty);
    }
}
