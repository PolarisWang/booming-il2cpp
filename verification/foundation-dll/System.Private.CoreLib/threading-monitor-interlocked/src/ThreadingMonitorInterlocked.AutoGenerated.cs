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

// Auto-generated skeleton for Monitor And Interlocked (family/System.Private.CoreLib/threading/monitor-interlocked).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ThreadingMonitorInterlocked
{
    // Monitor.Enter
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object()
    {
    // TODO: needs-manual — Enter with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object() { }

    // Monitor.Exit
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object()
    {
    // TODO: needs-manual — Exit with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object() { }

    // Monitor.TryEnter
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object()
    {
    var result = Monitor.TryEnter(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object() { _ = Monitor.TryEnter(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object() { _ = Monitor.TryEnter(42); }

    // Monitor.TryEnter
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32()
    {
    var result = Monitor.TryEnter(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32() { _ = Monitor.TryEnter(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32() { _ = Monitor.TryEnter(42, 42); }

    // Monitor.Pulse
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object()
    {
    // TODO: needs-manual — Pulse with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object() { }

    // Monitor.PulseAll
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object()
    {
    // TODO: needs-manual — PulseAll with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object() { }

    // Monitor.Wait
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object()
    {
    // TODO: needs-manual — Wait with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object() { }

    // Interlocked.Increment
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Increment with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32() { }

    // Interlocked.Decrement
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Decrement with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32() { }

    // Interlocked.Exchange
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Exchange with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32() { }

    // Interlocked.CompareExchange
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — CompareExchange with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32() { }

    // Interlocked.Add
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Add with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32() { }

    // Interlocked.MemoryBarrier
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void()
    {
    Interlocked.MemoryBarrier();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void() { Interlocked.MemoryBarrier(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void() { Interlocked.MemoryBarrier(); }

    // Volatile.Read
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32() { }
}
