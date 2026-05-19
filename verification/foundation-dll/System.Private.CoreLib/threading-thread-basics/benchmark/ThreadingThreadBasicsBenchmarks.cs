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

// Benchmark stubs for family/System.Private.CoreLib/threading/thread-basics
public static class ThreadingThreadBasicsBenchmarks
{
    // Benchmark Thread.get_ThreadState native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_get_ThreadState_System_Threading_ThreadState() { _ = Thread.CurrentThread.ThreadState; }
    // Benchmark Thread.get_Priority native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_get_Priority_System_Threading_ThreadPriority() { _ = Thread.CurrentThread.Priority; }
    // Benchmark Thread.set_Priority native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_set_Priority_System_Void_System_Threading_ThreadPriority() { Thread.CurrentThread.Priority = System.Threading.ThreadPriority.Normal; }
    // Benchmark Thread.get_IsBackground native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::get_IsBackground:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_get_IsBackground_System_Boolean() { _ = Thread.CurrentThread.IsBackground; }
    // Benchmark Thread.set_IsBackground native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_set_IsBackground_System_Void_System_Boolean() { Thread.CurrentThread.IsBackground = true; }
    // Benchmark Thread.get_IsThreadPoolThread native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_get_IsThreadPoolThread_System_Boolean() { _ = Thread.CurrentThread.IsThreadPoolThread; }
    // Benchmark Thread.Abort native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::Abort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_Abort_System_Void() { Thread.CurrentThread.Abort(); }
    // Benchmark Thread.Interrupt native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::Interrupt:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_Interrupt_System_Void() { Thread.CurrentThread.Interrupt(); }
    // Benchmark Thread.ResetAbort native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::ResetAbort:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_ResetAbort_System_Boolean() { }
    // Benchmark Thread.Yield native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_Yield_System_Boolean() { _ = Thread.Yield(); }
    // Benchmark Thread.Sleep native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingThreadBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Thread_Sleep_System_Void_System_Int32() { Thread.Sleep(42); }}
