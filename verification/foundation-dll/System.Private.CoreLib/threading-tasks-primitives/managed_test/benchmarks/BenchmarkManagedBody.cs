// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/threading/tasks-primitives
using System;

namespace Chaos.Benchmark.threading_tasks_primitives
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_System_Action()
        {
            BenchmarkChecksum += ((Task.Run(null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_1_System_Func_1()
        {
            BenchmarkChecksum += ((Task.Run(null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32()
        {
            BenchmarkChecksum += ((Task.Delay(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void()
        {
            BenchmarkChecksum += (int)(Task.CompletedTask.Wait());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32()
        {
            BenchmarkChecksum += ((Task.CompletedTask.Wait(42)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean()
        {
            BenchmarkChecksum += ((Task.CompletedTask.IsCompleted) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus()
        {
            BenchmarkChecksum += ((Task.CompletedTask.Status).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_ContinueWith_System_Threading_Tasks_Task_System_Action_System_Threading_Tasks_Task()
        {
            BenchmarkChecksum += ((Task.CompletedTask.ContinueWith(null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_WhenAll_System_Threading_Tasks_Task_System_Threading_Tasks_Task()
        {
            BenchmarkChecksum += ((Task.WhenAll(Array.Empty<System.Threading.Tasks.Task>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_WhenAny_System_Threading_Tasks_Task_System_Threading_Tasks_Task_System_Threading_Tasks_Task()
        {
            BenchmarkChecksum += ((Task.WhenAny(Array.Empty<System.Threading.Tasks.Task>())).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)
        public static void System_Private_CoreLib_System_Threading_Tasks_Task_FromResult_System_Threading_Tasks_Task_1_TResult()
        {
            BenchmarkChecksum += ((Task.FromResult(null!)).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Thread::Start:System.Void()
        public static void System_Private_CoreLib_System_Threading_Thread_Start_System_Void()
        {
            BenchmarkChecksum += (int)(Thread.CurrentThread.Start());
        }

        // System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
        public static void System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32()
        {
            BenchmarkChecksum += (int)(Thread.Sleep(42));
        }

        // System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()
        public static void System_Private_CoreLib_System_Threading_Thread_get_CurrentThread_System_Threading_Thread()
        {
            BenchmarkChecksum += ((Thread.CurrentThread).GetHashCode());
        }

        // System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()
        public static void System_Private_CoreLib_System_Threading_Thread_get_ManagedThreadId_System_Int32()
        {
            BenchmarkChecksum += Thread.CurrentThread.ManagedThreadId;
        }

    }
}
