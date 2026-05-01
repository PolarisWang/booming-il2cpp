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

// Auto-generated hot-update skeletons for FamilySystem.runtime.interopservicesFunctionPointersHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.runtime.interopservicesFunctionPointersHotUpdate
{
    // Purpose: Verify UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute::UnmanagedCallersOnlyAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_UnmanagedCallersOnlyAttribute_UnmanagedCallersOnlyAttribut() { _ = UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute:(); }}
