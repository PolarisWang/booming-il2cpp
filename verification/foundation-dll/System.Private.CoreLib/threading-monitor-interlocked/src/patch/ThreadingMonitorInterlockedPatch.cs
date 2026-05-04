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

public static partial class ThreadingMonitorInterlockedPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — Interlocked / Monitor
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Interlocked.Increment 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Interlocked_Increment()
    {
        int val = 0;
        _ = Interlocked.Increment(ref val);
    }

    /// <summary>
    /// Patch 侧验证 Interlocked.Exchange 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Interlocked_Exchange()
    {
        int val = 0;
        _ = Interlocked.Exchange(ref val, 1);
    }

    /// <summary>
    /// Patch 侧验证 Interlocked.MemoryBarrier 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Interlocked_MemoryBarrier()
    {
        Interlocked.MemoryBarrier();
    }
}
