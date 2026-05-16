// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/tasks/primitives
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Threading;
using System.Threading.Tasks;

public static partial class ThreadingTasksPrimitivesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)

    // [1] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)

    // [2] System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)

    // [3] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()

    // [4] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)

    // [5] System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()

    // [6] System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()

    // [7] System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})

    // [8] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])

    // [9] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])

    // [10] System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)

    // [11] System.Private.CoreLib/System.Threading.Thread::Start:System.Void()

    // [12] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)

    // [13] System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()
    public static void Subject_13()
    {
        if (((Thread.CurrentThread).GetHashCode()) != ((Thread.CurrentThread).GetHashCode())) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: CustomEntrySubject_0(); break;
            case 1: CustomEntrySubject_1(); break;
            case 2: CustomEntrySubject_2(); break;
            case 3: CustomEntrySubject_3(); break;
            case 4: CustomEntrySubject_4(); break;
            case 5: CustomEntrySubject_5(); break;
            case 6: CustomEntrySubject_6(); break;
            case 7: CustomEntrySubject_7(); break;
            case 8: CustomEntrySubject_8(); break;
            case 9: CustomEntrySubject_9(); break;
            case 10: CustomEntrySubject_10(); break;
            case 11: CustomEntrySubject_11(); break;
            case 12: CustomEntrySubject_12(); break;
            case 13: Subject_13(); break;
            case 14: CustomEntrySubject_14(); break;
        }
    }

}