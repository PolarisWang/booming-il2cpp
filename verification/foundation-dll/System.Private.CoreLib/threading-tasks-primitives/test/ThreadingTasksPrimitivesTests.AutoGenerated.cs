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

// Auto-generated skeleton for Tasks And Threading Primitives (family/System.Private.CoreLib/threading/tasks-primitives).
public partial class ThreadingTasksPrimitivesTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Run with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_System_Action()
    {
    // TODO: needs-manual — Run with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Run with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_1_System_Func_1()
    {
    // TODO: needs-manual — Run with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32()
    {
    _ = Task.Delay(42);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void()
    {
    Task.CompletedTask.Wait();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32()
    {
    var result = Task.CompletedTask.Wait(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean()
    {
    var result = Task.CompletedTask.IsCompleted;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus()
    {
    _ = Task.CompletedTask.Status;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ContinueWith with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_ContinueWith_System_Threading_Tasks_Task_System_Action_System_Threadi()
    {
    // TODO: needs-manual — ContinueWith with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — WhenAll with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_WhenAll_System_Threading_Tasks_Task_System_Threading_Tasks_Task()
    {
    // TODO: needs-manual — WhenAll with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — WhenAny with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_WhenAny_System_Threading_Tasks_Task_System_Threading_Tasks_Task_Syste()
    {
    // TODO: needs-manual — WhenAny with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — FromResult with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Tasks_Task_FromResult_System_Threading_Tasks_Task_1_TResult()
    {
    // TODO: needs-manual — FromResult with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Thread::Start:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Start with 0 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Thread_Start_System_Void()
    {
    // TODO: needs-manual — Start with 0 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32()
    {
    Thread.Sleep(42);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Thread_get_CurrentThread_System_Threading_Thread()
    {
    _ = Thread.CurrentThread;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Thread_get_ManagedThreadId_System_Int32()
    {
    var result = Thread.CurrentThread.ManagedThreadId;
    Xunit.Assert.NotNull((object)result);
    }}
