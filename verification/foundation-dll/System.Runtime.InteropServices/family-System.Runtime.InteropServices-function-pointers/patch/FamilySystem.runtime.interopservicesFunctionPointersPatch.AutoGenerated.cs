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

// Auto-generated patch-side skeletons for FamilySystem.runtime.interopservicesFunctionPointersPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.interopservicesFunctionPointersPatch
{
    // Purpose: Verify UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute::UnmanagedCallersOnlyAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_UnmanagedCallersOnlyAttribute_UnmanagedCallersOnlyAttribut() { _ = UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute:(); }}
