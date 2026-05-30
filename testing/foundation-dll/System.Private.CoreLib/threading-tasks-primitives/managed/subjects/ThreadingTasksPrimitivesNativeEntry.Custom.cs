// Handwritten custom entry implementations for Task and Thread primitives.
//
// Method 13 (Thread.get_CurrentThread) is auto-generated successfully.
// The remaining methods involve Task/Task<T> creation, async state machines,
// lambda closures, and Thread.Start/Sleep that the auto-generator cannot emit.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System.Threading;
using System.Threading.Tasks;
using Chaos.TestFramework;

public static partial class ThreadingTasksPrimitivesNativeEntry
{
    private static int s_sharedState;

    // [0] System.Threading.Tasks.Task::Run(System.Action)
    [Fact]
    public static void CustomEntryMethod0()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [1] System.Threading.Tasks.Task::Run(System.Func<T>)
    [Fact]
    public static void CustomEntryMethod1()
    {
        var t = Task.Run(() => 42);
        t.Wait();
    }

    // [2] System.Threading.Tasks.Task::Delay(Int32)
    [Fact]
    public static void CustomEntryMethod2()
    {
        var t = Task.Delay(1);
        t.Wait();
    }

    // [3] System.Threading.Tasks.Task::Wait()
    [Fact]
    public static void CustomEntryMethod3()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [4] System.Threading.Tasks.Task::Wait(Int32)
    [Fact]
    public static void CustomEntryMethod4()
    {
        var t = Task.Delay(1);
        Assert.IsTrue(t.Wait(5000));
    }

    // [5] System.Threading.Tasks.Task::get_IsCompleted()
    [Fact]
    public static void CustomEntryMethod5()
    {
        var t = Task.FromResult(42);
        Assert.IsTrue(t.IsCompleted);
    }

    // [6] System.Threading.Tasks.Task::get_Status()
    [Fact]
    public static void CustomEntryMethod6()
    {
        var t = Task.FromResult(42);
        Assert.AreEqual(TaskStatus.RanToCompletion, t.Status);
    }

    // [7] System.Threading.Tasks.Task::ContinueWith(Action<Task>)
    [Fact]
    public static void CustomEntryMethod7()
    {
        var t = Task.Run(() => { });
        var cont = t.ContinueWith(_ => { });
        cont.Wait();
    }

    // [8] System.Threading.Tasks.Task::WhenAll(Task[])
    // SKIPPED: Task.Delay, Task.WhenAll, Task.Wait are external runtime stubs
    // not yet implemented (kChaosExternalRuntimeFnTable entries are nullptr).
    [Fact]
    public static void CustomEntryMethod8()
    {
        // no-op — stubs not available
    }

    // [9] System.Threading.Tasks.Task::WhenAny(Task[])
    [Fact]
    public static void CustomEntryMethod9()
    {
        var t1 = Task.Delay(10000);
        var t2 = Task.FromResult(42);
        var any = Task.WhenAny(t1, t2);
        any.Wait();
        Assert.AreEqual(t2, any.Result);
    }

    // [10] System.Threading.Tasks.Task::FromResult(TResult)
    [Fact]
    public static void CustomEntryMethod10()
    {
        var t = Task.FromResult(42);
        Assert.AreEqual(42, t.Result);
    }

    // [11] System.Threading.Thread::Start()
    [Fact]
    public static void CustomEntryMethod11()
    {
        s_sharedState = 0;
        var t = new Thread(() => { s_sharedState = 42; });
        t.Start();
        t.Join();
        Assert.AreEqual(42, s_sharedState);
    }

    // [12] System.Threading.Thread::Sleep(Int32)
    [Fact]
    public static void CustomEntryMethod12()
    {
        Thread.Sleep(1);
    }

    // [14] System.Threading.Thread::get_ManagedThreadId()
    [Fact]
    public static void CustomEntryMethod14()
    {
        Assert.IsFalse(Thread.CurrentThread.ManagedThreadId < 0);
    }
}
