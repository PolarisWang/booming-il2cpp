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

// Auto-generated patch-side skeletons for ReflectionModulePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class ReflectionModulePatch
{
    // Purpose: Verify Module.get_FullyQualifiedName executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_get_FullyQualifiedName_System_String() { _ = Module.FullyQualifiedName; }
    // Purpose: Verify Module.get_Name executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_get_Name_System_String() { _ = Module.Name; }
    // Purpose: Verify Module.get_Assembly executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_get_Assembly_System_Reflection_Assembly() { _ = Module.Assembly; }
    // Purpose: Verify Module.GetType executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_GetType_System_Type_System_String() { _ = Module.GetType("hello"); }
    // Purpose: Verify Module.GetTypes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_GetTypes_System_Type() { _ = Module.GetTypes(); }
    // Purpose: Verify Module.GetCustomAttributes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Module_GetCustomAttributes_System_Object_System_Type() { _ = Module.GetCustomAttributes(typeof(byte)); }}
