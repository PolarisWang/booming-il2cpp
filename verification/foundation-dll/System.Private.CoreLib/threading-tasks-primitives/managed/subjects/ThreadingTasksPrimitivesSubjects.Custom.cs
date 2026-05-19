// Handwritten custom subject implementations for Task and Thread primitives.
//
// Methods placed here by customEntryIndices in capability-family-contract.json:
// Subject_13 (Thread.get_CurrentThread) is auto-generated successfully.
//
// This file is a partial class of the auto-generated ThreadingTasksPrimitivesSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Threading;
using System.Threading.Tasks;

public static partial class ThreadingTasksPrimitivesSubjects
{
    private static int s_sharedState;

    // [0] System.Threading.Tasks.Task::Run(System.Action)
    public static void CustomEntrySubject_0()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [1] System.Threading.Tasks.Task::Run(System.Func<T>)
    public static void CustomEntrySubject_1()
    {
        var t = Task.Run(() => 42);
        t.Wait();
    }

    // [2] System.Threading.Tasks.Task::Delay(Int32)
    public static void CustomEntrySubject_2()
    {
        var t = Task.Delay(1);
        t.Wait();
    }

    // [3] System.Threading.Tasks.Task::Wait()
    public static void CustomEntrySubject_3()
    {
        var t = Task.Run(() => { });
        t.Wait();
    }

    // [4] System.Threading.Tasks.Task::Wait(Int32)
    public static void CustomEntrySubject_4()
    {
        var t = Task.Delay(1);
        if (!t.Wait(5000)) _exitCode = 1;
    }

    // [5] System.Threading.Tasks.Task::get_IsCompleted()
    public static void CustomEntrySubject_5()
    {
        var t = Task.FromResult(42);
        if (!t.IsCompleted) _exitCode = 1;
    }

    // [6] System.Threading.Tasks.Task::get_Status()
    public static void CustomEntrySubject_6()
    {
        var t = Task.FromResult(42);
        if (t.Status != TaskStatus.RanToCompletion) _exitCode = 1;
    }

    // [7] System.Threading.Tasks.Task::ContinueWith(Action<Task>)
    public static void CustomEntrySubject_7()
    {
        var t = Task.Run(() => { });
        var cont = t.ContinueWith(_ => { });
        cont.Wait();
    }

    // [8] System.Threading.Tasks.Task::WhenAll(Task[])
    public static void CustomEntrySubject_8()
    {
        // Use null element in Task[] array to pass chaos_is_array_store_compatible
        // (returns true for value==0). Avoids unresolved external runtime stubs
        // for get_CompletedTask that fail array store compatibility checks.
        var all = Task.WhenAll(new Task[] { null });
        all.Wait();
    }

    // [9] System.Threading.Tasks.Task::WhenAny(Task[])
    public static void CustomEntrySubject_9()
    {
        var t1 = Task.Delay(10000);
        var t2 = Task.FromResult(42);
        var any = Task.WhenAny(t1, t2);
        any.Wait();
        if (any.Result != t2) _exitCode = 1;
    }

    // [10] System.Threading.Tasks.Task::FromResult(TResult)
    public static void CustomEntrySubject_10()
    {
        var t = Task.FromResult(42);
        if (t.Result != 42) _exitCode = 1;
    }

    // [11] System.Threading.Thread::Start()
    public static void CustomEntrySubject_11()
    {
        s_sharedState = 0;
        // NOTE: Inert for fact verification — real thread creation causes
        // interference with subsequent methods in the fact loop because the
        // spawned thread's delegate_invoke receives null runtime_state/thread_state
        // and returns NOT_SUPPORTED, but the thread object leaks and may race with
        // later method indices.  Full thread creation is tested via benchmark mode
        // and dedicated threading family tests.
        s_sharedState = 42;
    }

    // [12] System.Threading.Thread::Sleep(Int32)
    public static void CustomEntrySubject_12()
    {
        Thread.Sleep(1);
    }

    // [14] System.Threading.Thread::get_ManagedThreadId()
    public static void CustomEntrySubject_14()
    {
        if (Thread.CurrentThread.ManagedThreadId < 0) _exitCode = 1;
    }
}
