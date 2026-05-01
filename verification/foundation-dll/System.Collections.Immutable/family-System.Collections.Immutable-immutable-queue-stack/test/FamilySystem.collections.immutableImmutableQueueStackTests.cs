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

[CapabilityTest("family/System.Collections.Immutable/immutable-queue-stack", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class FamilySystem.collections.immutableImmutableQueueStackTests
{
    // AI-generated tests for family/System.Collections.Immutable/immutable-queue-stack

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Property_System_Void()
    {
        // Purpose: Verify ImmutableQueue.get_Property with typical input
            ImmutableQueue.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Property_System_Void()
    {
        // Purpose: Verify ImmutableStack.get_Property with typical input
            ImmutableStack.Property;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Value_System_Void()
    {
        // Purpose: Verify ImmutableQueue.get_Value with typical input
            ImmutableQueue.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Count_System_Void()
    {
        // Purpose: Verify ImmutableQueue.get_Count with typical input
            ImmutableQueue.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_IsEmpty_System_Void()
    {
        // Purpose: Verify ImmutableQueue.get_IsEmpty with typical input
            ImmutableQueue.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Key_System_Void()
    {
        // Purpose: Verify ImmutableQueue.get_Key with typical input
            ImmutableQueue.Key;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Value_System_Void()
    {
        // Purpose: Verify ImmutableStack.get_Value with typical input
            ImmutableStack.Value;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Count_System_Void()
    {
        // Purpose: Verify ImmutableStack.get_Count with typical input
            ImmutableStack.Count;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_IsEmpty_System_Void()
    {
        // Purpose: Verify ImmutableStack.get_IsEmpty with typical input
            ImmutableStack.IsEmpty;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Key_System_Void()
    {
        // Purpose: Verify ImmutableStack.get_Key with typical input
            ImmutableStack.Key;
    }

    // === mixed (smoke) ===

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_ImmutableQueue_Void_System_Object()
    {
        // Purpose: Smoke — ImmutableQueue.ImmutableQueue: with complex param(s)
            _ = ImmutableQueue.ImmutableQueue:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method1_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.Method1 with complex param(s)
            ImmutableQueue.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — ImmutableQueue.Method2 with complex param(s)
            ImmutableQueue.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Validate_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.Validate with complex param(s)
            ImmutableQueue.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_ImmutableStack_Void_System_Object()
    {
        // Purpose: Smoke — ImmutableStack.ImmutableStack: with complex param(s)
            _ = ImmutableStack.ImmutableStack:(42);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method1_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.Method1 with complex param(s)
            ImmutableStack.Method1(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method2_System_Void_P0_P1()
    {
        // Purpose: Smoke — ImmutableStack.Method2 with complex param(s)
            ImmutableStack.Method2(null!, null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Validate_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.Validate with complex param(s)
            ImmutableStack.Validate(null!);
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.set_Value with complex param(s)
            ImmutableQueue.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.set_Count with complex param(s)
            ImmutableQueue.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.set_IsEmpty with complex param(s)
            ImmutableQueue.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableQueue.set_Key with complex param(s)
            ImmutableQueue.Key = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Value_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.set_Value with complex param(s)
            ImmutableStack.Value = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Count_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.set_Count with complex param(s)
            ImmutableStack.Count = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_IsEmpty_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.set_IsEmpty with complex param(s)
            ImmutableStack.IsEmpty = null!;
    }

    [Fact]
    public void _System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Key_System_Void_P0()
    {
        // Purpose: Smoke — ImmutableStack.set_Key with complex param(s)
            ImmutableStack.Key = null!;
    }
}
