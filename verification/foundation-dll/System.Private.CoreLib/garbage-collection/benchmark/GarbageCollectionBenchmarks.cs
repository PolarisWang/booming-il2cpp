using System;
using System.Runtime.CompilerServices;
using System.Threading;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/garbage-collection
// GC performance scenarios: collect latency, allocation throughput, promotion.
public static class GarbageCollectionBenchmarks
{
    // ── GC.Collect() latency ───────────────────────────────────────────
    // Measures full GC pause by calling GC.Collect() with prior allocation
    // pressure to ensure the collection has work to do.
    [BenchmarkSubjectId("System.Private.CoreLib/System.GC::Collect:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_Collect()
    {
        for (int i = 0; i < 128; i++) { _ = new byte[256]; }
        GC.Collect();
    }

    // ── GC.WaitForPendingFinalizers() latency ─────────────────────────
    // Measures finalizer processing time after a GC.
    [BenchmarkSubjectId("System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_WaitForPendingFinalizers()
    {
        GC.Collect();
        GC.WaitForPendingFinalizers();
    }

    // ── GC.KeepAlive() overhead ────────────────────────────────────────
    // Measures the cost of GC.KeepAlive() in an allocation-heavy path.
    [BenchmarkSubjectId("System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_KeepAlive()
    {
        var obj = new byte[1024];
        GC.KeepAlive(obj);
    }

    // ── GC.GetGeneration() overhead ────────────────────────────────────
    // Measures gen-query cost for objects surviving nursery promotion.
    [BenchmarkSubjectId("System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_GetGeneration()
    {
        var obj = new byte[128];
        GC.Collect(0);
        _ = GC.GetGeneration(obj);
        GC.KeepAlive(obj);
    }

    // ── Array.CreateInstance allocation throughput ─────────────────────
    // Measures managed array allocation path (type handle lookup + GC alloc).
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_ArrayCreateInstance()
    {
        for (int i = 0; i < 64; i++) { _ = Array.CreateInstance(typeof(byte), 256); }
    }

    // ── Multi-thread allocation + GC pressure ──────────────────────────
    // Simulates concurrent allocation from thread pool — GC cooperativity.
    [BenchmarkSubjectId("System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GarbageCollection)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_GC_MultiThreadCollect()
    {
        var barrier = new Barrier(4);
        var done = new CountdownEvent(4);
        for (int t = 0; t < 4; t++)
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                barrier.SignalAndWait();
                for (int i = 0; i < 64; i++) { _ = new byte[128]; }
                done.Signal();
            });
        }
        done.Wait();
        GC.Collect();
    }
}
