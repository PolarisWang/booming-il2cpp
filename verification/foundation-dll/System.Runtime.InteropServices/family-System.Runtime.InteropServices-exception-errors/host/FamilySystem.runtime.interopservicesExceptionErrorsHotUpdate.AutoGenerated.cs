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

// Auto-generated hot-update skeletons for FamilySystem.runtime.interopservicesExceptionErrorsHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.runtime.interopservicesExceptionErrorsHotUpdate
{
    // Purpose: Verify DllNotFoundException.DllNotFoundException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void() { _ = DllNotFoundException.DllNotFoundException:(); }
    // Purpose: Verify DllNotFoundException.DllNotFoundException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object() { _ = DllNotFoundException.DllNotFoundException:(42); }
    // Purpose: Verify DllNotFoundException.DllNotFoundException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object_System_Object() { _ = DllNotFoundException.DllNotFoundException:(42, 42); }
    // Purpose: Verify COMException.COMException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void() { _ = COMException.COMException:(); }
    // Purpose: Verify COMException.COMException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object() { _ = COMException.COMException:(42); }
    // Purpose: Verify COMException.COMException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object_System_Object() { _ = COMException.COMException:(42, 42); }
    // Purpose: Verify COMException.ToString executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_ToString_System_Void() { COMException.ToString(); }
    // Purpose: Verify SEHException.SEHException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void() { _ = SEHException.SEHException:(); }
    // Purpose: Verify SEHException.SEHException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object() { _ = SEHException.SEHException:(42); }
    // Purpose: Verify SEHException.SEHException: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object_System_Object() { _ = SEHException.SEHException:(42, 42); }
    // Purpose: Verify SEHException.CanResume executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_CanResume_System_Void() { SEHException.CanResume(); }}
