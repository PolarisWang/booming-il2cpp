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

// Hot-update stubs for family/System.Private.CoreLib/dispatch/basic
public static class DispatchBasicHotUpdate
{
    // Verify DispatchBasicNativeEntry.Run after hot-update (patch side)
    [HotUpdateSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DispatchBasicNativeEntry_DispatchBasicNativeEntry_Run_System_Int32_System_Int32() { _ = DispatchBasicNativeEntry.Run(42); }
    // Verify DispatchBasicNativeEntry.TestIDispatchAdd after hot-update (patch side)
    [HotUpdateSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIDispatchAdd_System_Int32() { _ = DispatchBasicNativeEntry.TestIDispatchAdd(); }
    // Verify DispatchBasicNativeEntry.TestIDispatchSub after hot-update (patch side)
    [HotUpdateSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIDispatchSub_System_Int32() { _ = DispatchBasicNativeEntry.TestIDispatchSub(); }
    // Verify DispatchBasicNativeEntry.TestDualGetStatus after hot-update (patch side)
    [HotUpdateSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestDualGetStatus_System_Int32() { _ = DispatchBasicNativeEntry.TestDualGetStatus(); }
    // Verify DispatchBasicNativeEntry.TestIUnknownGetValue after hot-update (patch side)
    [HotUpdateSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIUnknownGetValue_System_Int32() { _ = DispatchBasicNativeEntry.TestIUnknownGetValue(); }}
