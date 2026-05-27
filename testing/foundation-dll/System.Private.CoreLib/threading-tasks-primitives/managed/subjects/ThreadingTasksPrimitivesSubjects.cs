// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/tasks/primitives
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ThreadingTasksPrimitivesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)
    public static void CustomEntrySubject_0()
    {
    }

    // [1] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)
    public static void CustomEntrySubject_2()
    {
    }

    // [3] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()
    public static void CustomEntrySubject_3()
    {
    }

    // [4] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)
    public static void CustomEntrySubject_4()
    {
    }

    // [5] System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()
    public static void CustomEntrySubject_5()
    {
    }

    // [6] System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()
    public static void CustomEntrySubject_6()
    {
    }

    // [7] System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})
    public static void CustomEntrySubject_7()
    {
    }

    // [8] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])
    public static void CustomEntrySubject_8()
    {
    }

    // [9] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])
    public static void CustomEntrySubject_9()
    {
    }

    // [10] System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.Private.CoreLib/System.Threading.Thread::Start:System.Void()
    public static void CustomEntrySubject_11()
    {
    }

    // [12] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void CustomEntrySubject_12()
    {
    }

    // [13] System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()
    public static void Subject_13()
    {
        try { var _ = ((Thread.CurrentThread).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()
    public static void CustomEntrySubject_14()
    {
    }

}