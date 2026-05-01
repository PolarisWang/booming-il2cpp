using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

[CapabilityTest("family/System.Collections.Immutable/immutable-interlocked", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class FamilySystem.collections.immutableImmutableInterlockedTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable-interlocked

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Property_System_Void()
    {
        // Purpose: Verify ImmutableInterlocked.get_Property with typical input
            ImmutableInterlocked.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Value_System_Void()
    {
        // Purpose: Verify ImmutableInterlocked.get_Value with typical input
            ImmutableInterlocked.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Count_System_Void()
    {
        // Purpose: Verify ImmutableInterlocked.get_Count with typical input
            ImmutableInterlocked.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_IsEmpty_System_Void()
    {
        // Purpose: Verify ImmutableInterlocked.get_IsEmpty with typical input
            ImmutableInterlocked.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_get_Key_System_Void()
    {
        // Purpose: Verify ImmutableInterlocked.get_Key with typical input
            ImmutableInterlocked.Key;
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_ImmutableInterlocked_Void_System_Object()
    {
        // Purpose: Smoke — ImmutableInterlocked.ImmutableInterlocked: with complex param(s)
            _ = ImmutableInterlocked.ImmutableInterlocked:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method1_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.Method1 with complex param(s)
            ImmutableInterlocked.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — ImmutableInterlocked.Method2 with complex param(s)
            ImmutableInterlocked.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Validate_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.Validate with complex param(s)
            ImmutableInterlocked.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.set_Value with complex param(s)
            ImmutableInterlocked.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.set_Count with complex param(s)
            ImmutableInterlocked.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.set_IsEmpty with complex param(s)
            ImmutableInterlocked.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableInterlocked.set_Key with complex param(s)
            ImmutableInterlocked.Key = null!;
    }
}
