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

// Auto-generated patch-side skeletons for FamilySystem.runtime.interopservicesGeneratedMarshallingPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.interopservicesGeneratedMarshallingPatch
{
    // Purpose: Verify AnsiStringMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ConvertToManaged_System_V() { AnsiStringMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify AnsiStringMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ConvertToUnmanaged_System() { AnsiStringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify AnsiStringMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_Free_System_Void_P0() { AnsiStringMarshaller.Free(null!); }
    // Purpose: Verify AnsiStringMarshaller+ManagedToUnmanagedIn.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_Free() { AnsiStringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Verify AnsiStringMarshaller+ManagedToUnmanagedIn.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_From() { AnsiStringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Verify AnsiStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_ToUn() { AnsiStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Verify AnsiStringMarshaller+ManagedToUnmanagedIn.get_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_get_() { AnsiStringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Verify AnsiStringMarshaller+ManagedToUnmanagedIn.set_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_set_() { AnsiStringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Verify ArrayMarshaller.AllocateContainerForManagedElements executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_AllocateContainerForManagedE() { ArrayMarshaller.AllocateContainerForManagedElements(null!, null!); }
    // Purpose: Verify ArrayMarshaller.AllocateContainerForUnmanagedElements executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_AllocateContainerForUnmanage() { ArrayMarshaller.AllocateContainerForUnmanagedElements(null!, null!); }
    // Purpose: Verify ArrayMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_Free_System_Void_P0() { ArrayMarshaller.Free(null!); }
    // Purpose: Verify ArrayMarshaller.GetManagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetManagedValuesDestination_() { ArrayMarshaller.GetManagedValuesDestination(null!); }
    // Purpose: Verify ArrayMarshaller.GetManagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetManagedValuesSource_Syste() { ArrayMarshaller.GetManagedValuesSource(null!); }
    // Purpose: Verify ArrayMarshaller.GetUnmanagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetUnmanagedValuesDestinatio() { ArrayMarshaller.GetUnmanagedValuesDestination(null!, null!); }
    // Purpose: Verify ArrayMarshaller.GetUnmanagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetUnmanagedValuesSource_Sys() { ArrayMarshaller.GetUnmanagedValuesSource(null!, null!); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_Free_Sy() { ArrayMarshaller`2+ManagedToUnmanagedIn.Free(); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_FromMan() { ArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetMana() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource(); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetPinn() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetPinn() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(null!); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetUnma() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination(); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_ToUnman() { ArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.get_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_get_Buf() { ArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Verify ArrayMarshaller`2+ManagedToUnmanagedIn.set_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_set_Buf() { ArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Verify BStrStringMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ConvertToManaged_System_V() { BStrStringMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify BStrStringMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ConvertToUnmanaged_System() { BStrStringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify BStrStringMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_Free_System_Void_P0() { BStrStringMarshaller.Free(null!); }
    // Purpose: Verify BStrStringMarshaller+ManagedToUnmanagedIn.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_Free() { BStrStringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Verify BStrStringMarshaller+ManagedToUnmanagedIn.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_From() { BStrStringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Verify BStrStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_ToUn() { BStrStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Verify BStrStringMarshaller+ManagedToUnmanagedIn.get_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_get_() { BStrStringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Verify BStrStringMarshaller+ManagedToUnmanagedIn.set_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_set_() { BStrStringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Verify ComExposedClassAttribute.ComExposedClassAttribute`1: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::ComExposedClassAttribute`1::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComExposedClassAttribute_1_ComExposedClassAttr() { _ = ComExposedClassAttribute.ComExposedClassAttribute`1:(); }
    // Purpose: Verify ComExposedClassAttribute.GetComInterfaceEntries executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComExposedClassAttribute_1_GetComInterfaceEntr() { ComExposedClassAttribute.GetComInterfaceEntries(null!); }
    // Purpose: Verify ComInterfaceMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_ConvertToManaged_Syst() { ComInterfaceMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify ComInterfaceMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_ConvertToUnmanaged_Sy() { ComInterfaceMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify ComInterfaceMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_Free_System_Void_P0() { ComInterfaceMarshaller.Free(null!); }
    // Purpose: Verify ComObject.FinalRelease executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComObject_FinalRelease_System_Void() { ComObject.FinalRelease(); }
    // Purpose: Verify ComObject.Finalize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::Finalize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComObject_Finalize_System_Void() { ComObject.Finalize(); }
    // Purpose: Verify ComVariant.As`1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::As`1:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_As_1_System_Void() { ComVariant.As`1(); }
    // Purpose: Verify ComVariant.CreateRaw`1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::CreateRaw`1:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_CreateRaw_1_System_Void_P0_P1() { ComVariant.CreateRaw`1(null!, null!); }
    // Purpose: Verify ComVariant.Create`1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Create`1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_Create_1_System_Void_P0() { ComVariant.Create`1(null!); }
    // Purpose: Verify ComVariant.Dispose executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Dispose:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_Dispose_System_Void() { ComVariant.Dispose(); }
    // Purpose: Verify ComVariant.GetRawDataRef`1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::GetRawDataRef`1:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_GetRawDataRef_1_System_Void() { ComVariant.GetRawDataRef`1(); }
    // Purpose: Verify ComVariant.get_Null executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_Null:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_get_Null_System_Void() { ComVariant.Null; }
    // Purpose: Verify ComVariant.get_VarType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_VarType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_get_VarType_System_Void() { ComVariant.VarType; }
    // Purpose: Verify ComVariant.set_Null executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_Null:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_set_Null_System_Void_P0() { ComVariant.Null = null!; }
    // Purpose: Verify ComVariant.set_VarType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_VarType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_set_VarType_System_Void_P0() { ComVariant.VarType = null!; }
    // Purpose: Verify ComVariantMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_ConvertToManaged_System_V() { ComVariantMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify ComVariantMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_ConvertToUnmanaged_System() { ComVariantMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify ComVariantMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_Free_System_Void_P0() { ComVariantMarshaller.Free(null!); }
    // Purpose: Verify ComVariantMarshaller+RefPropagate.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_Free_System_() { ComVariantMarshaller+RefPropagate.Free(); }
    // Purpose: Verify ComVariantMarshaller+RefPropagate.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_FromManaged_() { ComVariantMarshaller+RefPropagate.FromManaged(null!); }
    // Purpose: Verify ComVariantMarshaller+RefPropagate.FromUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_FromUnmanage() { ComVariantMarshaller+RefPropagate.FromUnmanaged(null!); }
    // Purpose: Verify ComVariantMarshaller+RefPropagate.ToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_ToManaged_Sy() { ComVariantMarshaller+RefPropagate.ToManaged(); }
    // Purpose: Verify ComVariantMarshaller+RefPropagate.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_ToUnmanaged_() { ComVariantMarshaller+RefPropagate.ToUnmanaged(); }
    // Purpose: Verify ExceptionAsDefaultMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsDefaultMarshaller_1_ConvertToUnmana() { ExceptionAsDefaultMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify ExceptionAsHResultMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsHResultMarshaller_1_ConvertToUnmana() { ExceptionAsHResultMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify ExceptionAsNaNMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsNaNMarshaller_1_ConvertToUnmanaged_() { ExceptionAsNaNMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify ExceptionAsVoidMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsVoidMarshaller_ConvertToUnmanaged_S() { ExceptionAsVoidMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify GeneratedComClassAttribute.GeneratedComClassAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::GeneratedComClassAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute_GeneratedComClassAt() { _ = GeneratedComClassAttribute.GeneratedComClassAttribute:(); }
    // Purpose: Verify GeneratedComInterfaceAttribute.GeneratedComInterfaceAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::GeneratedComInterfaceAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_GeneratedComInt() { _ = GeneratedComInterfaceAttribute.GeneratedComInterfaceAttribute:(); }
    // Purpose: Verify GeneratedComInterfaceAttribute.get_ExceptionToUnmanagedMarshaller executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_ExceptionTo() { GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller; }
    // Purpose: Verify GeneratedComInterfaceAttribute.get_Options executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_Options_Sys() { GeneratedComInterfaceAttribute.Options; }
    // Purpose: Verify GeneratedComInterfaceAttribute.get_StringMarshalling executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshalling; }
    // Purpose: Verify GeneratedComInterfaceAttribute.get_StringMarshallingCustomType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshallingCustomType; }
    // Purpose: Verify GeneratedComInterfaceAttribute.set_ExceptionToUnmanagedMarshaller executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_ExceptionTo() { GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller = null!; }
    // Purpose: Verify GeneratedComInterfaceAttribute.set_Options executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_Options_Sys() { GeneratedComInterfaceAttribute.Options = null!; }
    // Purpose: Verify GeneratedComInterfaceAttribute.set_StringMarshalling executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshalling = null!; }
    // Purpose: Verify GeneratedComInterfaceAttribute.set_StringMarshallingCustomType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshallingCustomType = null!; }
    // Purpose: Verify IComExposedClass.GetComInterfaceEntries executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IComExposedClass_GetComInterfaceEntries_System() { IComExposedClass.GetComInterfaceEntries(null!); }
    // Purpose: Verify IComExposedDetails.GetComInterfaceEntries executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IComExposedDetails_GetComInterfaceEntries_Syst() { IComExposedDetails.GetComInterfaceEntries(null!); }
    // Purpose: Verify IIUnknownCacheStrategy.Clear executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_Clear_System_Void_P0() { IIUnknownCacheStrategy.Clear(null!); }
    // Purpose: Verify IIUnknownCacheStrategy.ConstructTableInfo executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_ConstructTableInfo_Syst() { IIUnknownCacheStrategy.ConstructTableInfo(null!, null!, null!); }
    // Purpose: Verify IIUnknownCacheStrategy.TryGetTableInfo executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TryGetTableInfo_System_() { IIUnknownCacheStrategy.TryGetTableInfo(null!, null!); }
    // Purpose: Verify IIUnknownCacheStrategy.TrySetTableInfo executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TrySetTableInfo_System_() { IIUnknownCacheStrategy.TrySetTableInfo(null!, null!); }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.get_ManagedType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_ManagedTy() { IIUnknownCacheStrategy+TableInfo.ManagedType; }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.get_Table executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_Table_Sys() { IIUnknownCacheStrategy+TableInfo.Table; }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.get_ThisPtr executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_ThisPtr_S() { IIUnknownCacheStrategy+TableInfo.ThisPtr; }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.set_ManagedType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_ManagedTy() { IIUnknownCacheStrategy+TableInfo.ManagedType = null!; }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.set_Table executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_Table_Sys() { IIUnknownCacheStrategy+TableInfo.Table = null!; }
    // Purpose: Verify IIUnknownCacheStrategy+TableInfo.set_ThisPtr executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_ThisPtr_S() { IIUnknownCacheStrategy+TableInfo.ThisPtr = null!; }
    // Purpose: Verify IIUnknownDerivedDetails.get_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_Iid_System_Void() { IIUnknownDerivedDetails.Iid; }
    // Purpose: Verify IIUnknownDerivedDetails.get_Implementation executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_Implementation_Sys() { IIUnknownDerivedDetails.Implementation; }
    // Purpose: Verify IIUnknownDerivedDetails.get_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_ManagedVirtualMeth() { IIUnknownDerivedDetails.ManagedVirtualMethodTable; }
    // Purpose: Verify IIUnknownDerivedDetails.set_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_Iid_System_Void_P0() { IIUnknownDerivedDetails.Iid = null!; }
    // Purpose: Verify IIUnknownDerivedDetails.set_Implementation executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Implementation:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_Implementation_Sys() { IIUnknownDerivedDetails.Implementation = null!; }
    // Purpose: Verify IIUnknownDerivedDetails.set_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_ManagedVirtualMeth() { IIUnknownDerivedDetails.ManagedVirtualMethodTable = null!; }
    // Purpose: Verify IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceDetailsStrategy_GetComExpose() { IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(null!); }
    // Purpose: Verify IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceDetailsStrategy_GetIUnknownD() { IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(null!); }
    // Purpose: Verify IIUnknownInterfaceType.get_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_get_Iid_System_Void() { IIUnknownInterfaceType.Iid; }
    // Purpose: Verify IIUnknownInterfaceType.get_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_get_ManagedVirtualMetho() { IIUnknownInterfaceType.ManagedVirtualMethodTable; }
    // Purpose: Verify IIUnknownInterfaceType.set_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_set_Iid_System_Void_P0() { IIUnknownInterfaceType.Iid = null!; }
    // Purpose: Verify IIUnknownInterfaceType.set_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_set_ManagedVirtualMetho() { IIUnknownInterfaceType.ManagedVirtualMethodTable = null!; }
    // Purpose: Verify IIUnknownStrategy.CreateInstancePointer executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_CreateInstancePointer_System() { IIUnknownStrategy.CreateInstancePointer(null!); }
    // Purpose: Verify IIUnknownStrategy.QueryInterface executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_QueryInterface_System_Void_P() { IIUnknownStrategy.QueryInterface(null!, null!, null!); }
    // Purpose: Verify IIUnknownStrategy.Release executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_Release_System_Void_P0() { IIUnknownStrategy.Release(null!); }
    // Purpose: Verify IUnknownDerivedAttribute.IUnknownDerivedAttribute`2: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::IUnknownDerivedAttribute`2::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_IUnknownDerivedAttr() { _ = IUnknownDerivedAttribute.IUnknownDerivedAttribute`2:(); }
    // Purpose: Verify IUnknownDerivedAttribute.get_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_Iid_System_Void() { IUnknownDerivedAttribute.Iid; }
    // Purpose: Verify IUnknownDerivedAttribute.get_Implementation executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_Implementation_() { IUnknownDerivedAttribute.Implementation; }
    // Purpose: Verify IUnknownDerivedAttribute.get_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_ManagedVirtualM() { IUnknownDerivedAttribute.ManagedVirtualMethodTable; }
    // Purpose: Verify IUnknownDerivedAttribute.set_Iid executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_Iid_System_Void() { IUnknownDerivedAttribute.Iid = null!; }
    // Purpose: Verify IUnknownDerivedAttribute.set_Implementation executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Implementation:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_Implementation_() { IUnknownDerivedAttribute.Implementation = null!; }
    // Purpose: Verify IUnknownDerivedAttribute.set_ManagedVirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_ManagedVirtualM() { IUnknownDerivedAttribute.ManagedVirtualMethodTable = null!; }
    // Purpose: Verify IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnmanagedVirtualMethodTableProvider_GetVirtua() { IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(null!); }
    // Purpose: Verify MarshalUsingAttribute.MarshalUsingAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_MarshalUsingAttribute_Vo() { _ = MarshalUsingAttribute.MarshalUsingAttribute:(); }
    // Purpose: Verify MarshalUsingAttribute.MarshalUsingAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_MarshalUsingAttribute_Vo() { _ = MarshalUsingAttribute.MarshalUsingAttribute:(42); }
    // Purpose: Verify MarshalUsingAttribute.get_ConstantElementCount executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ConstantElementCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_ConstantElementCount() { MarshalUsingAttribute.ConstantElementCount; }
    // Purpose: Verify MarshalUsingAttribute.get_CountElementName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_CountElementName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_CountElementName_Sys() { MarshalUsingAttribute.CountElementName; }
    // Purpose: Verify MarshalUsingAttribute.get_ElementIndirectionDepth executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ElementIndirectionDepth:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_ElementIndirectionDe() { MarshalUsingAttribute.ElementIndirectionDepth; }
    // Purpose: Verify MarshalUsingAttribute.get_NativeType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_NativeType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_NativeType_System_Vo() { MarshalUsingAttribute.NativeType; }
    // Purpose: Verify MarshalUsingAttribute.set_ConstantElementCount executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ConstantElementCount:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_ConstantElementCount() { MarshalUsingAttribute.ConstantElementCount = null!; }
    // Purpose: Verify MarshalUsingAttribute.set_CountElementName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_CountElementName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_CountElementName_Sys() { MarshalUsingAttribute.CountElementName = null!; }
    // Purpose: Verify MarshalUsingAttribute.set_ElementIndirectionDepth executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ElementIndirectionDepth:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_ElementIndirectionDe() { MarshalUsingAttribute.ElementIndirectionDepth = null!; }
    // Purpose: Verify MarshalUsingAttribute.set_NativeType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_NativeType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_NativeType_System_Vo() { MarshalUsingAttribute.NativeType = null!; }
    // Purpose: Verify PointerArrayMarshaller.AllocateContainerForManagedElements executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_AllocateContainerForM() { PointerArrayMarshaller.AllocateContainerForManagedElements(null!, null!); }
    // Purpose: Verify PointerArrayMarshaller.AllocateContainerForUnmanagedElements executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_AllocateContainerForU() { PointerArrayMarshaller.AllocateContainerForUnmanagedElements(null!, null!); }
    // Purpose: Verify PointerArrayMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_Free_System_Void_P0() { PointerArrayMarshaller.Free(null!); }
    // Purpose: Verify PointerArrayMarshaller.GetManagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetManagedValuesDesti() { PointerArrayMarshaller.GetManagedValuesDestination(null!); }
    // Purpose: Verify PointerArrayMarshaller.GetManagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetManagedValuesSourc() { PointerArrayMarshaller.GetManagedValuesSource(null!); }
    // Purpose: Verify PointerArrayMarshaller.GetUnmanagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetUnmanagedValuesDes() { PointerArrayMarshaller.GetUnmanagedValuesDestination(null!, null!); }
    // Purpose: Verify PointerArrayMarshaller.GetUnmanagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetUnmanagedValuesSou() { PointerArrayMarshaller.GetUnmanagedValuesSource(null!, null!); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.Free(); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource(); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(null!); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination(); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.get_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Verify PointerArrayMarshaller`2+ManagedToUnmanagedIn.set_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Verify StrategyBasedComWrappers.StrategyBasedComWrappers: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::StrategyBasedComWrappers::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_StrategyBasedComWrapp() { _ = StrategyBasedComWrappers.StrategyBasedComWrappers:(); }
    // Purpose: Verify StrategyBasedComWrappers.ComputeVtables executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ComputeVtables:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_ComputeVtables_System() { StrategyBasedComWrappers.ComputeVtables(null!, null!, null!); }
    // Purpose: Verify StrategyBasedComWrappers.CreateCacheStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateCacheStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateCacheStrategy_S() { StrategyBasedComWrappers.CreateCacheStrategy(); }
    // Purpose: Verify StrategyBasedComWrappers.CreateDefaultCacheStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateDefaultCacheStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateDefaultCacheStr() { StrategyBasedComWrappers.CreateDefaultCacheStrategy(); }
    // Purpose: Verify StrategyBasedComWrappers.CreateObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateObject_System_V() { StrategyBasedComWrappers.CreateObject(null!, null!); }
    // Purpose: Verify StrategyBasedComWrappers.CreateObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateObject_System_V() { StrategyBasedComWrappers.CreateObject(null!, null!, null!, null!); }
    // Purpose: Verify StrategyBasedComWrappers.GetOrCreateIUnknownStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateIUnknownStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_GetOrCreateIUnknownSt() { StrategyBasedComWrappers.GetOrCreateIUnknownStrategy(); }
    // Purpose: Verify StrategyBasedComWrappers.GetOrCreateInterfaceDetailsStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateInterfaceDetailsStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_GetOrCreateInterfaceD() { StrategyBasedComWrappers.GetOrCreateInterfaceDetailsStrategy(); }
    // Purpose: Verify StrategyBasedComWrappers.ReleaseObjects executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ReleaseObjects:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_ReleaseObjects_System() { StrategyBasedComWrappers.ReleaseObjects(null!); }
    // Purpose: Verify StrategyBasedComWrappers.get_DefaultIUnknownInterfaceDetailsStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_get_DefaultIUnknownIn() { StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy; }
    // Purpose: Verify StrategyBasedComWrappers.get_DefaultIUnknownStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_get_DefaultIUnknownSt() { StrategyBasedComWrappers.DefaultIUnknownStrategy; }
    // Purpose: Verify StrategyBasedComWrappers.set_DefaultIUnknownInterfaceDetailsStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownInterfaceDetailsStrategy:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_set_DefaultIUnknownIn() { StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy = null!; }
    // Purpose: Verify StrategyBasedComWrappers.set_DefaultIUnknownStrategy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownStrategy:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_set_DefaultIUnknownSt() { StrategyBasedComWrappers.DefaultIUnknownStrategy = null!; }
    // Purpose: Verify UniqueComInterfaceMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_ConvertToManage() { UniqueComInterfaceMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify UniqueComInterfaceMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_ConvertToUnmana() { UniqueComInterfaceMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify UniqueComInterfaceMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_Free_System_Voi() { UniqueComInterfaceMarshaller.Free(null!); }
    // Purpose: Verify Utf16StringMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_ConvertToManaged_System_() { Utf16StringMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify Utf16StringMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_ConvertToUnmanaged_Syste() { Utf16StringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify Utf16StringMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_Free_System_Void_P0() { Utf16StringMarshaller.Free(null!); }
    // Purpose: Verify Utf16StringMarshaller.GetPinnableReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_GetPinnableReference_Sys() { Utf16StringMarshaller.GetPinnableReference(null!); }
    // Purpose: Verify Utf8StringMarshaller.ConvertToManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ConvertToManaged_System_V() { Utf8StringMarshaller.ConvertToManaged(null!); }
    // Purpose: Verify Utf8StringMarshaller.ConvertToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ConvertToUnmanaged_System() { Utf8StringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Verify Utf8StringMarshaller.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_Free_System_Void_P0() { Utf8StringMarshaller.Free(null!); }
    // Purpose: Verify Utf8StringMarshaller+ManagedToUnmanagedIn.Free executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_Free() { Utf8StringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Verify Utf8StringMarshaller+ManagedToUnmanagedIn.FromManaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_From() { Utf8StringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Verify Utf8StringMarshaller+ManagedToUnmanagedIn.ToUnmanaged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_ToUn() { Utf8StringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Verify Utf8StringMarshaller+ManagedToUnmanagedIn.get_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_get_() { Utf8StringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Verify Utf8StringMarshaller+ManagedToUnmanagedIn.set_BufferSize executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_set_() { Utf8StringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Verify VirtualMethodTableInfo.VirtualMethodTableInfo: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::VirtualMethodTableInfo::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_VirtualMethodTableInfo_() { _ = VirtualMethodTableInfo.VirtualMethodTableInfo:(42, 42); }
    // Purpose: Verify VirtualMethodTableInfo.Deconstruct executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_Deconstruct_System_Void() { VirtualMethodTableInfo.Deconstruct(null!, null!); }
    // Purpose: Verify VirtualMethodTableInfo.get_ThisPointer executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_get_ThisPointer_System_() { VirtualMethodTableInfo.ThisPointer; }
    // Purpose: Verify VirtualMethodTableInfo.get_VirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_get_VirtualMethodTable_() { VirtualMethodTableInfo.VirtualMethodTable; }
    // Purpose: Verify VirtualMethodTableInfo.set_ThisPointer executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_ThisPointer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_set_ThisPointer_System_() { VirtualMethodTableInfo.ThisPointer = null!; }
    // Purpose: Verify VirtualMethodTableInfo.set_VirtualMethodTable executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_VirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_set_VirtualMethodTable_() { VirtualMethodTableInfo.VirtualMethodTable = null!; }}
