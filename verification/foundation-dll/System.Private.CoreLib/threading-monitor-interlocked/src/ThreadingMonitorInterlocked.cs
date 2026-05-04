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

public partial class ThreadingMonitorInterlocked
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — Interlocked / Monitor
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Interlocked_Increment()
    {
        int val = 0;
        Interlocked.Increment(ref val);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Interlocked_Exchange()
    {
        int val = 0;
        Interlocked.Exchange(ref val, 1);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Interlocked_MemoryBarrier()
    {
        Interlocked.MemoryBarrier();
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — Interlocked / Monitor
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Interlocked_Increment()
    {
        int val = 0;
        Interlocked.Increment(ref val);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Interlocked_CompareExchange()
    {
        int val = 10;
        Interlocked.CompareExchange(ref val, 20, 10);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Monitor_TryEnter()
    {
        var lockObj = new object();
        _ = Monitor.TryEnter(lockObj);
    }
}
