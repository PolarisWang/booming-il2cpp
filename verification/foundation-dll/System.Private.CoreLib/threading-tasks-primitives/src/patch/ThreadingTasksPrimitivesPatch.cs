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

public static partial class ThreadingTasksPrimitivesPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — Task 原语
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Task.CompletedTask.IsCompleted 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Task_IsCompleted()
    {
        _ = Task.CompletedTask.IsCompleted;
    }

    /// <summary>
    /// Patch 侧验证 Task.Wait 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Task_Wait()
    {
        Task.CompletedTask.Wait();
    }

    /// <summary>
    /// Patch 侧验证 Thread.get_CurrentThread 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingTasksPrimitives)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Thread_CurrentThread()
    {
        _ = Thread.CurrentThread;
    }
}
