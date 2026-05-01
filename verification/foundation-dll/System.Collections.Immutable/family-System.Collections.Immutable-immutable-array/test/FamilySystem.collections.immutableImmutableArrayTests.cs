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

[CapabilityTest("family/System.Collections.Immutable/immutable-array", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class FamilySystem.collections.immutableImmutableArrayTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable-array

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Property_System_Void()
    {
        // Purpose: Verify ImmutableArray.get_Property with typical input
            ImmutableArray.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Value_System_Void()
    {
        // Purpose: Verify ImmutableArray.get_Value with typical input
            ImmutableArray.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Count_System_Void()
    {
        // Purpose: Verify ImmutableArray.get_Count with typical input
            ImmutableArray.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_IsEmpty_System_Void()
    {
        // Purpose: Verify ImmutableArray.get_IsEmpty with typical input
            ImmutableArray.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_get_Key_System_Void()
    {
        // Purpose: Verify ImmutableArray.get_Key with typical input
            ImmutableArray.Key;
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_ImmutableArray_Void_System_Object()
    {
        // Purpose: Smoke — ImmutableArray.ImmutableArray: with complex param(s)
            _ = ImmutableArray.ImmutableArray:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Method1_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.Method1 with complex param(s)
            ImmutableArray.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — ImmutableArray.Method2 with complex param(s)
            ImmutableArray.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_Validate_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.Validate with complex param(s)
            ImmutableArray.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.set_Value with complex param(s)
            ImmutableArray.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.set_Count with complex param(s)
            ImmutableArray.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.set_IsEmpty with complex param(s)
            ImmutableArray.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableArray_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableArray.set_Key with complex param(s)
            ImmutableArray.Key = null!;
    }
}
