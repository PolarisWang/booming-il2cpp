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

// Auto-generated patch-side skeletons for ReflectionActivationPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ReflectionActivationPatch
{
    // Purpose: Verify Activator.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type() { _ = Activator.CreateInstance(typeof(byte)); }
    // Purpose: Verify Activator.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Object() { _ = Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>()); }
    // Purpose: Verify Activator.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_T() { _ = Activator.CreateInstance(); }
    // Purpose: Verify Activator.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_R() { _ = Activator.CreateInstance(typeof(byte), null!, null!, Array.Empty<System.Object>(), null!); }
    // Purpose: Verify Array.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }
    // Purpose: Verify Array.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }
    // Purpose: Verify Array.CreateInstance executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()); }
    // Purpose: Verify ConstructorInfo.Invoke executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { _ = typeof(object).GetConstructor(System.Type.EmptyTypes)!.Invoke(null); }}
