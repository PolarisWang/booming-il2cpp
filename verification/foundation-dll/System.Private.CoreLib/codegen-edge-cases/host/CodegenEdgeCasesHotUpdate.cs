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

// Hot-update stubs for family/System.Private.CoreLib/codegen/codegen-edge-cases
public static class CodegenEdgeCasesHotUpdate
{
    // Verify multi-latch loop after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_0:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Subject_0() { CodegenEdgeCasesSubjects.Subject_0(); }

    // Verify switch fallthrough after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_1:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Subject_1() { CodegenEdgeCasesSubjects.Subject_1(0); }

    // Verify deep EH nesting after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_2:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Subject_2() { CodegenEdgeCasesSubjects.Subject_2(); }

    // Verify finally control flow after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_3:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Subject_3() { CodegenEdgeCasesSubjects.Subject_3(5); }
}
