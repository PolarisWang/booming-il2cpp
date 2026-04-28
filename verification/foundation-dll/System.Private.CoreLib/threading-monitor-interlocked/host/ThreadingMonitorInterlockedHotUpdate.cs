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

// Hot-update stubs for family/System.Private.CoreLib/threading/monitor-interlocked
public static class ThreadingMonitorInterlockedHotUpdate
{
    // Verify Monitor.Enter after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object() { }
    // Verify Monitor.Exit after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object() { }
    // Verify Monitor.TryEnter after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object() { _ = Monitor.TryEnter(42); }
    // Verify Monitor.TryEnter after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32() { _ = Monitor.TryEnter(42, 42); }
    // Verify Monitor.Pulse after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object() { }
    // Verify Monitor.PulseAll after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object() { }
    // Verify Monitor.Wait after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object() { }
    // Verify Interlocked.Increment after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32() { }
    // Verify Interlocked.Decrement after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32() { }
    // Verify Interlocked.Exchange after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32() { }
    // Verify Interlocked.CompareExchange after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32() { }
    // Verify Interlocked.Add after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32() { }
    // Verify Interlocked.MemoryBarrier after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void() { Interlocked.MemoryBarrier(); }
    // Verify Volatile.Read after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32() { }}
