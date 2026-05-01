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

// Auto-generated hot-update skeletons for FamilySystem.security.principal.windowsSecurityIdentifierHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.security.principal.windowsSecurityIdentifierHotUpdate
{
    // Purpose: Verify SecurityIdentifier.SecurityIdentifier: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::SecurityIdentifier::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_SecurityIdentifier_Void_System_Object() { _ = SecurityIdentifier.SecurityIdentifier:(42); }
    // Purpose: Verify SecurityIdentifier.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Property_System_Void() { SecurityIdentifier.Property; }
    // Purpose: Verify SecurityIdentifier.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method1_System_Void_P0() { SecurityIdentifier.Method1(null!); }
    // Purpose: Verify SecurityIdentifier.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method2_System_Void_P0_P1() { SecurityIdentifier.Method2(null!, null!); }
    // Purpose: Verify SecurityIdentifier.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Validate_System_Void_P0() { SecurityIdentifier.Validate(null!); }
    // Purpose: Verify SecurityIdentifier.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Value_System_Void() { SecurityIdentifier.Value; }
    // Purpose: Verify SecurityIdentifier.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Value_System_Void_P0() { SecurityIdentifier.Value = null!; }
    // Purpose: Verify SecurityIdentifier.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Count_System_Void() { SecurityIdentifier.Count; }
    // Purpose: Verify SecurityIdentifier.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Count_System_Void_P0() { SecurityIdentifier.Count = null!; }
    // Purpose: Verify SecurityIdentifier.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_IsEmpty_System_Void() { SecurityIdentifier.IsEmpty; }
    // Purpose: Verify SecurityIdentifier.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_IsEmpty_System_Void_P0() { SecurityIdentifier.IsEmpty = null!; }
    // Purpose: Verify SecurityIdentifier.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Key_System_Void() { SecurityIdentifier.Key; }
    // Purpose: Verify SecurityIdentifier.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Key_System_Void_P0() { SecurityIdentifier.Key = null!; }}
