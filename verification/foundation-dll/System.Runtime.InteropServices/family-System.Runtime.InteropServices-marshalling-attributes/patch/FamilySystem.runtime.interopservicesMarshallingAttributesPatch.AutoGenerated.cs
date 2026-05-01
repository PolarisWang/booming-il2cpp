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

// Auto-generated patch-side skeletons for FamilySystem.runtime.interopservicesMarshallingAttributesPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.interopservicesMarshallingAttributesPatch
{
    // Purpose: Verify MarshalAsAttribute.MarshalAsAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::MarshalAsAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_MarshalAsAttribute_Void_System_Object() { _ = MarshalAsAttribute.MarshalAsAttribute:(42); }
    // Purpose: Verify MarshalAsAttribute.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_get_Value_System_Void() { MarshalAsAttribute.Value; }
    // Purpose: Verify MarshalAsAttribute.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_MarshalAsAttribute_set_Value_System_Void_P0() { MarshalAsAttribute.Value = null!; }
    // Purpose: Verify OptionalAttribute.OptionalAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.OptionalAttribute::OptionalAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_OptionalAttribute_OptionalAttribute_Void() { _ = OptionalAttribute.OptionalAttribute:(); }
    // Purpose: Verify PreserveSigAttribute.PreserveSigAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.PreserveSigAttribute::PreserveSigAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_PreserveSigAttribute_PreserveSigAttribute_Void() { _ = PreserveSigAttribute.PreserveSigAttribute:(); }}
