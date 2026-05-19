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

// Hot-update stubs for family/System.Private.CoreLib/interface/dispatch
public static class InterfaceDispatchPatch
{
    // Verify InterfaceDispatchNativeEntry.Run after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_Run_System_Int32_System_Int32() { _ = InterfaceDispatchNativeEntry.Run(42); }
    // Verify InterfaceDispatchNativeEntry.TestSingleImplSimple after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplSimple_System_Int32() { _ = InterfaceDispatchNativeEntry.TestSingleImplSimple(); }
    // Verify InterfaceDispatchNativeEntry.TestSingleImplDefault after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplDefault_System_Int32() { _ = InterfaceDispatchNativeEntry.TestSingleImplDefault(); }
    // Verify InterfaceDispatchNativeEntry.TestMultiImplCalc after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestMultiImplCalc_System_Int32() { _ = InterfaceDispatchNativeEntry.TestMultiImplCalc(); }
    // Verify InterfaceDispatchNativeEntry.TestIsCheck after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestIsCheck_System_Int32() { _ = InterfaceDispatchNativeEntry.TestIsCheck(); }
    // Verify InterfaceDispatchNativeEntry.TestAsCheck after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestAsCheck_System_Int32() { _ = InterfaceDispatchNativeEntry.TestAsCheck(); }
    // Verify InterfaceDispatchNativeEntry.TestDiamondBase after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondBase_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondBase(); }
    // Verify InterfaceDispatchNativeEntry.TestDiamondDerived after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondDerived_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondDerived(); }
    // Verify InterfaceDispatchNativeEntry.TestDiamondMulti after hot-update (host side)
    [HotUpdateSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondMulti_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondMulti(); }}
