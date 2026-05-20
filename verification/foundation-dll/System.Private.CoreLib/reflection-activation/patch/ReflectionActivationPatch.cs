using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/reflection/activation
public static class ReflectionActivationPatch
{
    // Verify Activator.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type() { _ = Activator.CreateInstance(typeof(byte)); }
    // Verify Activator.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Object() { _ = Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>()); }
    // Verify Activator.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_T() { }
    // Verify Activator.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_R() { _ = Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }
    // Verify Array.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }
    // Verify Array.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }
    // Verify Array.CreateInstance after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()); }
    // Verify ConstructorInfo.Invoke after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { _ = typeof(byte).GetConstructors()[0].Invoke(new object[0]); }}
