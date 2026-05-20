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

// Hot-update stubs for family/System.Private.CoreLib/pinvoke/dllimport
public static class PinvokeDllimportHotUpdate
{
    // Verify PinvokeDllimportNativeEntry.TestGetCurrentProcess after hot-update (patch side)
    [HotUpdateSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetCurrentProcess_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetCurrentProcess(); }
    // Verify PinvokeDllimportNativeEntry.TestGetCurrentThread after hot-update (patch side)
    [HotUpdateSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetCurrentThread_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetCurrentThread(); }
    // Verify PinvokeDllimportNativeEntry.TestGetProcessId after hot-update (patch side)
    [HotUpdateSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetProcessId_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetProcessId(); }
    // Verify PinvokeDllimportNativeEntry.TestGetModuleHandle after hot-update (patch side)
    [HotUpdateSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetModuleHandle_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetModuleHandle(); }}
