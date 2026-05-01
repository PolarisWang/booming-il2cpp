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

// Auto-generated patch-side skeletons for FamilySystem.security.principal.windowsTokenAccessPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.security.principal.windowsTokenAccessPatch
{
    // Purpose: Verify SafeAccessTokenHandle.SafeAccessTokenHandle: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::SafeAccessTokenHandle::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_SafeAccessTokenHandle_Void_System_Ob() { _ = SafeAccessTokenHandle.SafeAccessTokenHandle:(42); }
    // Purpose: Verify SafeAccessTokenHandle.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Property_System_Void() { SafeAccessTokenHandle.Property; }
    // Purpose: Verify SafeAccessTokenHandle.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Method1_System_Void_P0() { SafeAccessTokenHandle.Method1(null!); }
    // Purpose: Verify SafeAccessTokenHandle.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Method2_System_Void_P0_P1() { SafeAccessTokenHandle.Method2(null!, null!); }
    // Purpose: Verify SafeAccessTokenHandle.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Validate_System_Void_P0() { SafeAccessTokenHandle.Validate(null!); }
    // Purpose: Verify TokenAccessLevels.TokenAccessLevels: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::TokenAccessLevels::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_TokenAccessLevels_Void_System_Object() { _ = TokenAccessLevels.TokenAccessLevels:(42); }
    // Purpose: Verify TokenAccessLevels.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Property_System_Void() { TokenAccessLevels.Property; }
    // Purpose: Verify TokenAccessLevels.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Method1_System_Void_P0() { TokenAccessLevels.Method1(null!); }
    // Purpose: Verify TokenAccessLevels.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Method2_System_Void_P0_P1() { TokenAccessLevels.Method2(null!, null!); }
    // Purpose: Verify TokenAccessLevels.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Validate_System_Void_P0() { TokenAccessLevels.Validate(null!); }
    // Purpose: Verify SafeAccessTokenHandle.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Value_System_Void() { SafeAccessTokenHandle.Value; }
    // Purpose: Verify SafeAccessTokenHandle.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Value_System_Void_P0() { SafeAccessTokenHandle.Value = null!; }
    // Purpose: Verify SafeAccessTokenHandle.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Count_System_Void() { SafeAccessTokenHandle.Count; }
    // Purpose: Verify SafeAccessTokenHandle.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Count_System_Void_P0() { SafeAccessTokenHandle.Count = null!; }
    // Purpose: Verify SafeAccessTokenHandle.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_IsEmpty_System_Void() { SafeAccessTokenHandle.IsEmpty; }
    // Purpose: Verify SafeAccessTokenHandle.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_IsEmpty_System_Void_P0() { SafeAccessTokenHandle.IsEmpty = null!; }
    // Purpose: Verify SafeAccessTokenHandle.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Key_System_Void() { SafeAccessTokenHandle.Key; }
    // Purpose: Verify SafeAccessTokenHandle.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Key_System_Void_P0() { SafeAccessTokenHandle.Key = null!; }
    // Purpose: Verify TokenAccessLevels.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Value_System_Void() { TokenAccessLevels.Value; }
    // Purpose: Verify TokenAccessLevels.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Value_System_Void_P0() { TokenAccessLevels.Value = null!; }
    // Purpose: Verify TokenAccessLevels.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Count_System_Void() { TokenAccessLevels.Count; }
    // Purpose: Verify TokenAccessLevels.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Count_System_Void_P0() { TokenAccessLevels.Count = null!; }
    // Purpose: Verify TokenAccessLevels.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_IsEmpty_System_Void() { TokenAccessLevels.IsEmpty; }
    // Purpose: Verify TokenAccessLevels.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_IsEmpty_System_Void_P0() { TokenAccessLevels.IsEmpty = null!; }
    // Purpose: Verify TokenAccessLevels.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Key_System_Void() { TokenAccessLevels.Key; }
    // Purpose: Verify TokenAccessLevels.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Key_System_Void_P0() { TokenAccessLevels.Key = null!; }}
