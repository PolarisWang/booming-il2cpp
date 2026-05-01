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

// Auto-generated hot-update skeletons for FamilySystem.runtime.interopservicesSecureStringMarshalHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.runtime.interopservicesSecureStringMarshalHotUpdate
{
    // Purpose: Verify SecureStringMarshal.SecureStringToCoTaskMemAnsi executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemAnsi_System_Void_P0() { SecureStringMarshal.SecureStringToCoTaskMemAnsi(null!); }
    // Purpose: Verify SecureStringMarshal.SecureStringToCoTaskMemUnicode executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemUnicode_System_Void_P0() { SecureStringMarshal.SecureStringToCoTaskMemUnicode(null!); }
    // Purpose: Verify SecureStringMarshal.SecureStringToGlobalAllocAnsi executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocAnsi_System_Void_P0() { SecureStringMarshal.SecureStringToGlobalAllocAnsi(null!); }
    // Purpose: Verify SecureStringMarshal.SecureStringToGlobalAllocUnicode executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocUnicode_System_Void_P0() { SecureStringMarshal.SecureStringToGlobalAllocUnicode(null!); }}
