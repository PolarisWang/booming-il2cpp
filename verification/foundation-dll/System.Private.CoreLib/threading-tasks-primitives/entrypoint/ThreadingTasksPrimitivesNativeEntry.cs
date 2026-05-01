// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/tasks/primitives
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Threading;
using System.Threading.Tasks;

public static class ThreadingTasksPrimitivesNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1) could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)
    static int Method2()
    {
        return Task.Delay(42).GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()
    static int Method3()
    {
        Task.CompletedTask.Wait();
        return (int)0;
    }

    // [4] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)
    static int Method4()
    {
        return (Task.CompletedTask.Wait(42) ? 1 : 0);
    }

    // [5] System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()
    static int Method5()
    {
        return (Task.CompletedTask.IsCompleted ? 1 : 0);
    }

    // [6] System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()
    static int Method6()
    {
        return Task.CompletedTask.Status.GetHashCode();
    }

    // [7] System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task}) could not be auto-generated
        return 0;
    }

    // [8] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])
    static int Method8()
    {
        return Task.WhenAll(new System.Threading.Tasks.Task[0]).GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[]) could not be auto-generated
        return 0;
    }

    // [10] System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)
    static int Method10()
    {
        // TODO: System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult) could not be auto-generated
        return 0;
    }

    // [11] System.Private.CoreLib/System.Threading.Thread::Start:System.Void()
    static int Method11()
    {
        // TODO: System.Private.CoreLib/System.Threading.Thread::Start:System.Void() could not be auto-generated
        return 0;
    }

    // [12] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    static int Method12()
    {
        Thread.Sleep(42);
        return (int)0;
    }

    // [13] System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()
    static int Method13()
    {
        return Thread.CurrentThread.GetHashCode();
    }

    // [14] System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()
    static int Method14()
    {
        return Thread.CurrentThread.ManagedThreadId;
    }

}