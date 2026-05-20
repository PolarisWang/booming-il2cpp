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

public static partial class ThreadingTasksPrimitivesNativeEntry
{
    public static int _exitCode;
    private static int s_sharedState;

    // [0] System.Threading.Tasks.Task::Run(System.Action)
    public static void CustomEntryMethod0()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [1] System.Threading.Tasks.Task::Run(System.Func<T>)
    public static void CustomEntryMethod1()
    {
        var t = Task.Run(() => 42);
        t.Wait();
    }

    // [2] System.Threading.Tasks.Task::Delay(Int32)
    public static void CustomEntryMethod2()
    {
        var t = Task.Delay(1);
        t.Wait();
    }

    // [3] System.Threading.Tasks.Task::Wait()
    public static void CustomEntryMethod3()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [4] System.Threading.Tasks.Task::Wait(Int32)
    public static void CustomEntryMethod4()
    {
        var t = Task.Delay(1);
        if (!t.Wait(5000)) _exitCode = 1;
    }

    // [5] System.Threading.Tasks.Task::get_IsCompleted()
    public static void CustomEntryMethod5()
    {
        var t = Task.FromResult(42);
        if (!t.IsCompleted) _exitCode = 1;
    }

    // [6] System.Threading.Tasks.Task::get_Status()
    public static void CustomEntryMethod6()
    {
        var t = Task.FromResult(42);
        if (t.Status != TaskStatus.RanToCompletion) _exitCode = 1;
    }

    // [7] System.Threading.Tasks.Task::ContinueWith(Action<Task>)
    public static void CustomEntryMethod7()
    {
        var t = Task.Run(() => { });
        var cont = t.ContinueWith(_ => { });
        cont.Wait();
    }

    // [8] System.Threading.Tasks.Task::WhenAll(Task[])
    public static void CustomEntryMethod8()
    {
        var t1 = Task.Delay(1);
        var t2 = Task.Delay(1);
        var all = Task.WhenAll(t1, t2);
        all.Wait();
    }

    // [9] System.Threading.Tasks.Task::WhenAny(Task[])
    public static void CustomEntryMethod9()
    {
        var t1 = Task.Delay(10000);
        var t2 = Task.FromResult(42);
        var any = Task.WhenAny(t1, t2);
        any.Wait();
        if (any.Result != t2) _exitCode = 1;
    }

    // [10] System.Threading.Tasks.Task::FromResult(TResult)
    public static void CustomEntryMethod10()
    {
        var t = Task.FromResult(42);
        if (t.Result != 42) _exitCode = 1;
    }

    // [11] System.Threading.Thread::Start()
    public static void CustomEntryMethod11()
    {
        s_sharedState = 0;
        var t = new Thread(() => { s_sharedState = 42; });
        t.Start();
        t.Join();
        if (s_sharedState != 42) _exitCode = 1;
    }

    // [12] System.Threading.Thread::Sleep(Int32)
    public static void CustomEntryMethod12()
    {
        Thread.Sleep(1);
    }

    // [14] System.Threading.Thread::get_ManagedThreadId()
    public static void CustomEntryMethod14()
    {
        if (Thread.CurrentThread.ManagedThreadId < 0) _exitCode = 1;
    }
}
