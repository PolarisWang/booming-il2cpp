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

// Auto-generated hot-update skeletons for FamilySystem.security.principal.windowsNtAccountHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.security.principal.windowsNtAccountHotUpdate
{
    // Purpose: Verify NTAccount.NTAccount: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::NTAccount::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_NTAccount_Void_System_Object() { _ = NTAccount.NTAccount:(42); }
    // Purpose: Verify NTAccount.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Property_System_Void() { NTAccount.Property; }
    // Purpose: Verify NTAccount.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_Method1_System_Void_P0() { NTAccount.Method1(null!); }
    // Purpose: Verify NTAccount.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_Method2_System_Void_P0_P1() { NTAccount.Method2(null!, null!); }
    // Purpose: Verify NTAccount.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_Validate_System_Void_P0() { NTAccount.Validate(null!); }
    // Purpose: Verify NTAccount.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Value_System_Void() { NTAccount.Value; }
    // Purpose: Verify NTAccount.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Value_System_Void_P0() { NTAccount.Value = null!; }
    // Purpose: Verify NTAccount.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Count_System_Void() { NTAccount.Count; }
    // Purpose: Verify NTAccount.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Count_System_Void_P0() { NTAccount.Count = null!; }
    // Purpose: Verify NTAccount.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_IsEmpty_System_Void() { NTAccount.IsEmpty; }
    // Purpose: Verify NTAccount.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_IsEmpty_System_Void_P0() { NTAccount.IsEmpty = null!; }
    // Purpose: Verify NTAccount.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_get_Key_System_Void() { NTAccount.Key; }
    // Purpose: Verify NTAccount.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.NTAccount::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Security_Principal_Windows_System_Security_Principal_NTAccount_set_Key_System_Void_P0() { NTAccount.Key = null!; }}
