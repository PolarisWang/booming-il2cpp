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

[CapabilityTest("family/System.Collections.Immutable/frozen-collections", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class FamilySystem.collections.immutableFrozenCollectionsTests
{
    // AI-generated tests for family/System.Collections.Immutable/frozen-collections

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Property_System_Void()
    {
        // Purpose: Verify FrozenDictionary.get_Property with typical input
            FrozenDictionary.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Property_System_Void()
    {
        // Purpose: Verify FrozenSet.get_Property with typical input
            FrozenSet.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Value_System_Void()
    {
        // Purpose: Verify FrozenDictionary.get_Value with typical input
            FrozenDictionary.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Count_System_Void()
    {
        // Purpose: Verify FrozenDictionary.get_Count with typical input
            FrozenDictionary.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_IsEmpty_System_Void()
    {
        // Purpose: Verify FrozenDictionary.get_IsEmpty with typical input
            FrozenDictionary.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_get_Key_System_Void()
    {
        // Purpose: Verify FrozenDictionary.get_Key with typical input
            FrozenDictionary.Key;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Value_System_Void()
    {
        // Purpose: Verify FrozenSet.get_Value with typical input
            FrozenSet.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Count_System_Void()
    {
        // Purpose: Verify FrozenSet.get_Count with typical input
            FrozenSet.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_IsEmpty_System_Void()
    {
        // Purpose: Verify FrozenSet.get_IsEmpty with typical input
            FrozenSet.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_get_Key_System_Void()
    {
        // Purpose: Verify FrozenSet.get_Key with typical input
            FrozenSet.Key;
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_FrozenDictionary_Void_System_Object()
    {
        // Purpose: Smoke — FrozenDictionary.FrozenDictionary: with complex param(s)
            _ = FrozenDictionary.FrozenDictionary:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method1_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.Method1 with complex param(s)
            FrozenDictionary.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — FrozenDictionary.Method2 with complex param(s)
            FrozenDictionary.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_Validate_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.Validate with complex param(s)
            FrozenDictionary.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_FrozenSet_Void_System_Object()
    {
        // Purpose: Smoke — FrozenSet.FrozenSet: with complex param(s)
            _ = FrozenSet.FrozenSet:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method1_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.Method1 with complex param(s)
            FrozenSet.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — FrozenSet.Method2 with complex param(s)
            FrozenSet.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_Validate_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.Validate with complex param(s)
            FrozenSet.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.set_Value with complex param(s)
            FrozenDictionary.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.set_Count with complex param(s)
            FrozenDictionary.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.set_IsEmpty with complex param(s)
            FrozenDictionary.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenDictionary_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — FrozenDictionary.set_Key with complex param(s)
            FrozenDictionary.Key = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.set_Value with complex param(s)
            FrozenSet.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.set_Count with complex param(s)
            FrozenSet.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.set_IsEmpty with complex param(s)
            FrozenSet.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Frozen_FrozenSet_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — FrozenSet.set_Key with complex param(s)
            FrozenSet.Key = null!;
    }
}
