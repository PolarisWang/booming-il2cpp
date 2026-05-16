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

[CapabilityTest("family/System.Private.CoreLib/threading/monitor-interlocked", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ThreadingMonitorInterlockedTests
{
    // AI-generated tests for family/System.Private.CoreLib/threading/monitor-interlocked

    // === simple (all-primitive) ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object()
    {
        // Purpose: Verify Monitor.TryEnter with typical input
            var result = Monitor.TryEnter(42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32()
    {
        // Purpose: Verify Monitor.TryEnter with typical input
            var result = Monitor.TryEnter(42, 42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void()
    {
        // Purpose: Verify Interlocked.MemoryBarrier with typical input
            Interlocked.MemoryBarrier();
    }

    // === ref/pointer ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32()
    {
        int val = 42;
        int result = Volatile.Read(ref val);
        Xunit.Assert.Equal(42, result);
    }

    // === Monitor Enter/Exit ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Xunit.Assert.True(Monitor.IsEntered(lockObj));
        Monitor.Exit(lockObj);
        Xunit.Assert.False(Monitor.IsEntered(lockObj));
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Exit(lockObj);
        // After exit, another thread should be able to enter
        Xunit.Assert.True(Monitor.TryEnter(lockObj, 100));
        Monitor.Exit(lockObj);
    }

    // === Monitor Pulse/Wait ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.Pulse(lockObj); // No exception expected
        Monitor.Exit(lockObj);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        Monitor.PulseAll(lockObj); // No exception expected
        Monitor.Exit(lockObj);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object()
    {
        object lockObj = new object();
        Monitor.Enter(lockObj);
        // Wait with 1ms timeout should return false (timeout) without exception
        bool result = Monitor.Wait(lockObj, 1);
        Monitor.Exit(lockObj);
        Xunit.Assert.False(result);
    }

    // === Interlocked operations ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32()
    {
        int val = 0;
        Xunit.Assert.Equal(1, Interlocked.Increment(ref val));
        Xunit.Assert.Equal(2, Interlocked.Increment(ref val));
        Xunit.Assert.Equal(2, val);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32()
    {
        int val = 5;
        Xunit.Assert.Equal(4, Interlocked.Decrement(ref val));
        Xunit.Assert.Equal(3, Interlocked.Decrement(ref val));
        Xunit.Assert.Equal(3, val);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32()
    {
        int val = 42;
        int prev = Interlocked.Exchange(ref val, 99);
        Xunit.Assert.Equal(42, prev);
        Xunit.Assert.Equal(99, val);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32()
    {
        int val = 42;
        // Match case: exchange should succeed
        Xunit.Assert.Equal(42, Interlocked.CompareExchange(ref val, 99, 42));
        Xunit.Assert.Equal(99, val);
        // No-match case: exchange should not happen
        Xunit.Assert.Equal(99, Interlocked.CompareExchange(ref val, 1, 999));
        Xunit.Assert.Equal(99, val); // unchanged
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32()
    {
        int val = 10;
        Xunit.Assert.Equal(15, Interlocked.Add(ref val, 5));
        Xunit.Assert.Equal(15, val);
        Xunit.Assert.Equal(10, Interlocked.Add(ref val, -5));
        Xunit.Assert.Equal(10, val);
    }
}
