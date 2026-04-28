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

// Auto-generated benchmark skeletons for ThreadingMonitorInterlockedBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ThreadingMonitorInterlockedBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Monitor.Enter with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Monitor.Exit with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Monitor.TryEnter with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object() { _ = Monitor.TryEnter(42); }
    // Purpose: Benchmark native-runtime performance of Monitor.TryEnter with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32() { _ = Monitor.TryEnter(42, 42); }
    // Purpose: Benchmark native-runtime performance of Monitor.Pulse with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Monitor.PulseAll with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Monitor.Wait with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.Increment with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.Decrement with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.Exchange with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.CompareExchange with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.Add with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Benchmark native-runtime performance of Interlocked.MemoryBarrier with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void() { Interlocked.MemoryBarrier(); }
    // Purpose: Benchmark native-runtime performance of Volatile.Read with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    public static void Benchmark_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32() { }}
