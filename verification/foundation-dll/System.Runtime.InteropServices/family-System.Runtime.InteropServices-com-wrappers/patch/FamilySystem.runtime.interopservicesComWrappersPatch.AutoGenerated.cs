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

// Auto-generated patch-side skeletons for FamilySystem.runtime.interopservicesComWrappersPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.interopservicesComWrappersPatch
{
    // Purpose: Verify ComWrappers.ComWrappers: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComWrappers::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComWrappers_Void() { _ = ComWrappers.ComWrappers:(); }
    // Purpose: Verify ComWrappers.ComputeVtables executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComputeVtables:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComputeVtables_System_Void_P0_P1_P2() { ComWrappers.ComputeVtables(null!, null!, null!); }
    // Purpose: Verify ComWrappers.CreateObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1() { ComWrappers.CreateObject(null!, null!); }
    // Purpose: Verify ComWrappers.CreateObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1_P2_P3() { ComWrappers.CreateObject(null!, null!, null!, null!); }
    // Purpose: Verify ComWrappers.GetIUnknownImpl executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetIUnknownImpl_System_Void_P0_P1_P2() { ComWrappers.GetIUnknownImpl(null!, null!, null!); }
    // Purpose: Verify ComWrappers.GetOrCreateComInterfaceForObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateComInterfaceForObject_System_Void_P() { ComWrappers.GetOrCreateComInterfaceForObject(null!, null!); }
    // Purpose: Verify ComWrappers.GetOrCreateObjectForComInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0() { ComWrappers.GetOrCreateObjectForComInstance(null!, null!); }
    // Purpose: Verify ComWrappers.GetOrCreateObjectForComInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0() { ComWrappers.GetOrCreateObjectForComInstance(null!, null!, null!); }
    // Purpose: Verify ComWrappers.GetOrRegisterObjectForComInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_() { ComWrappers.GetOrRegisterObjectForComInstance(null!, null!, null!); }
    // Purpose: Verify ComWrappers.GetOrRegisterObjectForComInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_() { ComWrappers.GetOrRegisterObjectForComInstance(null!, null!, null!, null!); }
    // Purpose: Verify ComWrappers.RegisterForMarshalling executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForMarshalling_System_Void_P0() { ComWrappers.RegisterForMarshalling(null!); }
    // Purpose: Verify ComWrappers.RegisterForTrackerSupport executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForTrackerSupport_System_Void_P0() { ComWrappers.RegisterForTrackerSupport(null!); }
    // Purpose: Verify ComWrappers.ReleaseObjects executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ReleaseObjects:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ReleaseObjects_System_Void_P0() { ComWrappers.ReleaseObjects(null!); }
    // Purpose: Verify ComWrappers.TryGetComInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetComInstance_System_Void_P0_P1() { ComWrappers.TryGetComInstance(null!, null!); }
    // Purpose: Verify ComWrappers.TryGetObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetObject_System_Void_P0_P1() { ComWrappers.TryGetObject(null!, null!); }
    // Purpose: Verify ComWrappers+ComInterfaceDispatch.GetInstance`1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers+ComInterfaceDispatch::GetInstance`1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComInterfaceDispatch_GetInstance_1_System_Void() { ComWrappers+ComInterfaceDispatch.GetInstance`1(null!); }}
