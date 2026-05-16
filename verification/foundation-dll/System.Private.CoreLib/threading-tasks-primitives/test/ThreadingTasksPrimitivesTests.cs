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

[CapabilityTest("family/System.Private.CoreLib/threading/tasks-primitives", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class ThreadingTasksPrimitivesTests
{
    // AI-generated tests for family/System.Private.CoreLib/threading/tasks-primitives

    // === smoke tests ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Thread_get_CurrentThread_System_Threading_Thread()
    {
        // Purpose: Smoke — Thread.get_CurrentThread with complex param(s)
            _ = Thread.CurrentThread;
    }

    // === Task.Run ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_System_Action()
    {
        bool ran = false;
        var t = Task.Run(() => { ran = true; });
        t.Wait();
        Xunit.Assert.True(ran);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_1_System_Func_1()
    {
        var t = Task.Run(() => 42);
        t.Wait();
        Xunit.Assert.Equal(42, t.Result);
    }

    // === Task.Delay ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32()
    {
        var t = Task.Delay(1);
        Xunit.Assert.True(t.Wait(5000));
    }

    // === Task.Wait ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void()
    {
        var t = Task.Run(() => { });
        t.Wait(); // Should return without exception
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32()
    {
        var t = Task.Run(() => { });
        Xunit.Assert.True(t.Wait(5000));
    }

    // === Task status ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean()
    {
        var t = Task.FromResult(42);
        Xunit.Assert.True(t.IsCompleted);
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus()
    {
        var t = Task.FromResult(42);
        Xunit.Assert.Equal(TaskStatus.RanToCompletion, t.Status);
    }

    // === Task continuation ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_ContinueWith_System_Threading_Tasks_Task_System_Action_System_Threadi()
    {
        bool continued = false;
        var t = Task.Run(() => { });
        var cont = t.ContinueWith(_ => { continued = true; });
        cont.Wait();
        Xunit.Assert.True(continued);
    }

    // === Task.WhenAll / WhenAny ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_WhenAll_System_Threading_Tasks_Task_System_Threading_Tasks_Task()
    {
        var t1 = Task.Delay(1);
        var t2 = Task.FromResult(42);
        var all = Task.WhenAll(t1, t2);
        Xunit.Assert.True(all.Wait(5000));
    }

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_WhenAny_System_Threading_Tasks_Task_System_Threading_Tasks_Task_Syste()
    {
        var t1 = Task.Delay(10000);
        var t2 = Task.FromResult(42);
        var any = Task.WhenAny(t1, t2);
        any.Wait();
        Xunit.Assert.Equal(t2, any.Result); // t2 should complete first
    }

    // === Task.FromResult ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Tasks_Task_FromResult_System_Threading_Tasks_Task_1_TResult()
    {
        var t = Task.FromResult(42);
        Xunit.Assert.True(t.IsCompleted);
        Xunit.Assert.Equal(42, t.Result);
    }

    // === Thread.Start ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Thread_Start_System_Void()
    {
        int result = 0;
        var t = new Thread(() => { result = 42; });
        t.Start();
        t.Join();
        Xunit.Assert.Equal(42, result);
    }

    // === Thread.Sleep ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32()
    {
        Thread.Sleep(1); // Should return without exception
    }

    // === Thread.get_ManagedThreadId ===

    [Fact]
    public void _System_Private_CoreLib_System_Threading_Thread_get_ManagedThreadId_System_Int32()
    {
        int id = Thread.CurrentThread.ManagedThreadId;
        Xunit.Assert.True(id >= 0);
    }
}
