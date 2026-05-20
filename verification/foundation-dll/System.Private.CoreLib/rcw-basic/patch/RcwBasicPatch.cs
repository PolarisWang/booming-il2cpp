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

// Hot-update stubs for family/System.Private.CoreLib/rcw/basic
public static class RcwBasicPatch
{
    // Verify RcwBasicNativeEntry.Run after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_Run_System_Int32_System_Int32() { _ = RcwBasicNativeEntry.Run(42); }
    // Verify RcwBasicNativeEntry.TestRcwRoundTripIdentity after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwRoundTripIdentity_System_Int32() { _ = RcwBasicNativeEntry.TestRcwRoundTripIdentity(); }
    // Verify RcwBasicNativeEntry.TestRcwRoundTripQi after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwRoundTripQi_System_Int32() { _ = RcwBasicNativeEntry.TestRcwRoundTripQi(); }
    // Verify RcwBasicNativeEntry.TestRcwMultipleWrappers after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwMultipleWrappers_System_Int32() { _ = RcwBasicNativeEntry.TestRcwMultipleWrappers(); }
    // Verify RcwBasicNativeEntry.TestRcwQiUnknownInterface after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwQiUnknownInterface_System_Int32() { _ = RcwBasicNativeEntry.TestRcwQiUnknownInterface(); }
    // Verify RcwBasicNativeEntry.TestRcwVtableMethodCall after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwVtableMethodCall_System_Int32() { _ = RcwBasicNativeEntry.TestRcwVtableMethodCall(); }
    // Verify RcwBasicNativeEntry.TestRcwDirectVtable after hot-update (host side)
    [HotUpdateSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwDirectVtable_System_Int32() { _ = RcwBasicNativeEntry.TestRcwDirectVtable(); }}
