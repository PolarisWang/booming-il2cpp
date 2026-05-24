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

// Hot-update stubs for family/System.Private.CoreLib/error/info/basic
public static class ErrorInfoBasicPatch
{
    // Verify ErrorInfoBasicNativeEntry.Run after hot-update (host side)
    [HotUpdateSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_Run_System_Int32_System_Int32() { _ = ErrorInfoBasicNativeEntry.Run(42); }
    // Verify ErrorInfoBasicNativeEntry.TestGetExceptionForHR after hot-update (host side)
    [HotUpdateSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR(); }
    // Verify ErrorInfoBasicNativeEntry.TestGetHRForException after hot-update (host side)
    [HotUpdateSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetHRForException_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetHRForException(); }
    // Verify ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK after hot-update (host side)
    [HotUpdateSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK(); }
    // Verify ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero after hot-update (host side)
    [HotUpdateSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero(); }}
