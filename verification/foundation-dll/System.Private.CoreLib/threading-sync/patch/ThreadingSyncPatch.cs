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

// Hot-update stubs for family/System.Private.CoreLib/threading/sync
public static class ThreadingSyncPatch
{
    // Verify SemaphoreSlim..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SemaphoreSlim_ctor_System_Void_System_Int32() { new SemaphoreSlim(42); }
    // Verify SemaphoreSlim..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SemaphoreSlim_ctor_System_Void_System_Int32_System_Int32() { new SemaphoreSlim(42, 42); }
    // Verify SemaphoreSlim.Wait after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::Wait:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SemaphoreSlim_Wait_System_Void() { SemaphoreSlim.Wait(); }
    // Verify SemaphoreSlim.Release after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::Release:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SemaphoreSlim_Release_System_Int32() { _ = SemaphoreSlim.Release(); }
    // Verify SemaphoreSlim.get_CurrentCount after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SemaphoreSlim::get_CurrentCount:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SemaphoreSlim_get_CurrentCount_System_Int32() { _ = SemaphoreSlim.CurrentCount; }
    // Verify ManualResetEvent..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_ManualResetEvent_ctor_System_Void_System_Boolean() { new ManualResetEvent(true); }
    // Verify ManualResetEvent.Set after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::Set:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_ManualResetEvent_Set_System_Boolean() { _ = ManualResetEvent.Set(); }
    // Verify ManualResetEvent.Reset after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::Reset:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_ManualResetEvent_Reset_System_Boolean() { _ = ManualResetEvent.Reset(); }
    // Verify ManualResetEvent.WaitOne after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.ManualResetEvent::WaitOne:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_ManualResetEvent_WaitOne_System_Boolean() { _ = ManualResetEvent.WaitOne(); }
    // Verify AutoResetEvent..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_AutoResetEvent_ctor_System_Void_System_Boolean() { new AutoResetEvent(true); }
    // Verify AutoResetEvent.Set after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::Set:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_AutoResetEvent_Set_System_Boolean() { _ = AutoResetEvent.Set(); }
    // Verify AutoResetEvent.Reset after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::Reset:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_AutoResetEvent_Reset_System_Boolean() { _ = AutoResetEvent.Reset(); }
    // Verify AutoResetEvent.WaitOne after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.AutoResetEvent::WaitOne:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_AutoResetEvent_WaitOne_System_Boolean() { _ = AutoResetEvent.WaitOne(); }
    // Verify SpinLock..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SpinLock::.ctor:System.Void(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SpinLock_ctor_System_Void_System_Boolean() { new SpinLock(true); }
    // Verify SpinLock.Enter after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SpinLock::Enter:System.Void(System.Boolean&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SpinLock_Enter_System_Void_System_Boolean() { }
    // Verify SpinLock.Exit after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SpinLock::Exit:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SpinLock_Exit_System_Void() { SpinLock.Exit(); }
    // Verify SpinWait.SpinOnce after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Threading.SpinWait::SpinOnce:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingSync)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Threading_SpinWait_SpinOnce_System_Void() { SpinWait.SpinOnce(); }}
