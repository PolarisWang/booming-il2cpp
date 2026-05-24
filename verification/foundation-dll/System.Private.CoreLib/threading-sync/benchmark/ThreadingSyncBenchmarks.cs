using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/threading/sync
public static class ThreadingSyncBenchmarks
{
    // Benchmark SemaphoreSlim..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SemaphoreSlim_ctor_System_Void_System_Int32() { new SemaphoreSlim(42); }
    // Benchmark SemaphoreSlim..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SemaphoreSlim_ctor_System_Void_System_Int32_System_Int32() { new SemaphoreSlim(42, 42); }
    // Benchmark SemaphoreSlim.Wait native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SemaphoreSlim_Wait_System_Void() { SemaphoreSlim.Wait(); }
    // Benchmark SemaphoreSlim.Release native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::Release:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SemaphoreSlim_Release_System_Int32() { _ = SemaphoreSlim.Release(); }
    // Benchmark SemaphoreSlim.get_CurrentCount native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::get_CurrentCount:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SemaphoreSlim_get_CurrentCount_System_Int32() { _ = SemaphoreSlim.CurrentCount; }
    // Benchmark ManualResetEvent..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_ManualResetEvent_ctor_System_Void_System_Boolean() { new ManualResetEvent(true); }
    // Benchmark ManualResetEvent.Set native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::Set:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_ManualResetEvent_Set_System_Boolean() { _ = ManualResetEvent.Set(); }
    // Benchmark ManualResetEvent.Reset native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::Reset:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_ManualResetEvent_Reset_System_Boolean() { _ = ManualResetEvent.Reset(); }
    // Benchmark ManualResetEvent.WaitOne native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::WaitOne:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_ManualResetEvent_WaitOne_System_Boolean() { _ = ManualResetEvent.WaitOne(); }
    // Benchmark AutoResetEvent..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_AutoResetEvent_ctor_System_Void_System_Boolean() { new AutoResetEvent(true); }
    // Benchmark AutoResetEvent.Set native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::Set:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_AutoResetEvent_Set_System_Boolean() { _ = AutoResetEvent.Set(); }
    // Benchmark AutoResetEvent.Reset native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::Reset:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_AutoResetEvent_Reset_System_Boolean() { _ = AutoResetEvent.Reset(); }
    // Benchmark AutoResetEvent.WaitOne native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::WaitOne:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_AutoResetEvent_WaitOne_System_Boolean() { _ = AutoResetEvent.WaitOne(); }
    // Benchmark SpinLock..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SpinLock::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SpinLock_ctor_System_Void_System_Boolean() { new SpinLock(true); }
    // Benchmark SpinLock.Enter native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SpinLock::Enter:System.Void(System.Boolean&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SpinLock_Enter_System_Void_System_Boolean() { }
    // Benchmark SpinLock.Exit native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SpinLock::Exit:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SpinLock_Exit_System_Void() { SpinLock.Exit(); }
    // Benchmark SpinWait.SpinOnce native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.SpinWait::SpinOnce:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_SpinWait_SpinOnce_System_Void() { SpinWait.SpinOnce(); }}
