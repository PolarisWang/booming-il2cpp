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

// Hot-update stubs for family/System.Private.CoreLib/threading/tasks-primitives
public static class ThreadingTasksPrimitivesPatch
{
    // Verify Task.Run after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_System_Action() { _ = Task.Run(new Action(() => {})); }
    // Verify Task.Run after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_Run_System_Threading_Tasks_Task_1_System_Func_1() { _ = Task.Run(new Action(() => {})); }
    // Verify Task.Delay after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_Delay_System_Threading_Tasks_Task_System_Int32() { _ = Task.Delay(42); }
    // Verify Task.Wait after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Void() { Task.CompletedTask.Wait(); }
    // Verify Task.Wait after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_Wait_System_Boolean_System_Int32() { _ = Task.CompletedTask.Wait(42); }
    // Verify Task.get_IsCompleted after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_get_IsCompleted_System_Boolean() { _ = Task.CompletedTask.IsCompleted; }
    // Verify Task.get_Status after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_get_Status_System_Threading_Tasks_TaskStatus() { _ = Task.CompletedTask.Status; }
    // Verify Task.ContinueWith after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_ContinueWith_System_Threading_Tasks_Task_System_Action_System_Threadi() { _ = Task.CompletedTask.ContinueWith(new Action<Task>(_ => {})); }
    // Verify Task.WhenAll after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_WhenAll_System_Threading_Tasks_Task_System_Threading_Tasks_Task() { _ = Task.WhenAll(Array.Empty<System.Threading.Tasks.Task>()); }
    // Verify Task.WhenAny after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_WhenAny_System_Threading_Tasks_Task_System_Threading_Tasks_Task_Syste() { _ = Task.WhenAny(Array.Empty<System.Threading.Tasks.Task>()); }
    // Verify Task.FromResult after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Tasks_Task_FromResult_System_Threading_Tasks_Task_1_TResult() { }
    // Verify Thread.Start after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::Start:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Thread_Start_System_Void() { new System.Threading.Thread(() => {}).Start(); }
    // Verify Thread.Sleep after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32() { Thread.Sleep(42); }
    // Verify Thread.get_CurrentThread after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Thread_get_CurrentThread_System_Threading_Thread() { _ = Thread.CurrentThread; }
    // Verify Thread.get_ManagedThreadId after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Thread_get_ManagedThreadId_System_Int32() { _ = Thread.CurrentThread.ManagedThreadId; }}
