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

// Hot-update stubs for family/System.Private.CoreLib/ccw/basic
public static class CcwBasicPatch
{
    // Verify CcwBasicNativeEntry.Run after hot-update (host side)
    [HotUpdateSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CcwBasicNativeEntry_CcwBasicNativeEntry_Run_System_Int32_System_Int32() { _ = CcwBasicNativeEntry.Run(42); }
    // Verify CcwBasicNativeEntry.TestSimpleMathAdd after hot-update (host side)
    [HotUpdateSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CcwBasicNativeEntry_CcwBasicNativeEntry_TestSimpleMathAdd_System_Int32() { _ = CcwBasicNativeEntry.TestSimpleMathAdd(); }
    // Verify CcwBasicNativeEntry.TestSimpleMathMul after hot-update (host side)
    [HotUpdateSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CcwBasicNativeEntry_CcwBasicNativeEntry_TestSimpleMathMul_System_Int32() { _ = CcwBasicNativeEntry.TestSimpleMathMul(); }
    // Verify CcwBasicNativeEntry.TestConstantValue after hot-update (host side)
    [HotUpdateSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestConstantValue:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CcwBasicNativeEntry_CcwBasicNativeEntry_TestConstantValue_System_Int32() { _ = CcwBasicNativeEntry.TestConstantValue(); }
    // Verify CcwBasicNativeEntry.TestDualInterface after hot-update (host side)
    [HotUpdateSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestDualInterface:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_CcwBasicNativeEntry_CcwBasicNativeEntry_TestDualInterface_System_Int32() { _ = CcwBasicNativeEntry.TestDualInterface(); }}
