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

// Auto-generated patch-side skeletons for ThreadingMonitorInterlockedPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ThreadingMonitorInterlockedPatch
{
    // Purpose: Verify Monitor.Enter executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object() { }
    // Purpose: Verify Monitor.Exit executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object() { }
    // Purpose: Verify Monitor.TryEnter executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object() { _ = Monitor.TryEnter(42); }
    // Purpose: Verify Monitor.TryEnter executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32() { _ = Monitor.TryEnter(42, 42); }
    // Purpose: Verify Monitor.Pulse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object() { }
    // Purpose: Verify Monitor.PulseAll executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object() { }
    // Purpose: Verify Monitor.Wait executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object() { }
    // Purpose: Verify Interlocked.Increment executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32() { }
    // Purpose: Verify Interlocked.Decrement executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32() { }
    // Purpose: Verify Interlocked.Exchange executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Verify Interlocked.CompareExchange executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Verify Interlocked.Add executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32() { }
    // Purpose: Verify Interlocked.MemoryBarrier executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void() { Interlocked.MemoryBarrier(); }
    // Purpose: Verify Volatile.Read executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32() { }}
