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

public partial class ThreadingTasksPrimitives
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — Task 原语
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Task_IsCompleted()
    {
        _ = Task.CompletedTask.IsCompleted;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Task_Wait()
    {
        Task.CompletedTask.Wait();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Thread_CurrentThread()
    {
        _ = Thread.CurrentThread;
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — Task 原语
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Task_GetStatus()
    {
        _ = Task.CompletedTask.Status;
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Task_Delay()
    {
        _ = Task.Delay(1);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Thread_ManagedThreadId()
    {
        _ = Thread.CurrentThread.ManagedThreadId;
    }
}
