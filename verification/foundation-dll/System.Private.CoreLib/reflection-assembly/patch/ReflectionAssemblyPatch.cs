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

// Hot-update stubs for family/System.Private.CoreLib/reflection/assembly
public static class ReflectionAssemblyPatch
{
    // Verify Assembly.GetExecutingAssembly after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetExecutingAssembly_System_Reflection_Assembly() { _ = Assembly.GetExecutingAssembly(); }
    // Verify Assembly.GetCallingAssembly after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetCallingAssembly_System_Reflection_Assembly() { _ = Assembly.GetCallingAssembly(); }
    // Verify Assembly.GetEntryAssembly after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetEntryAssembly_System_Reflection_Assembly() { _ = Assembly.GetEntryAssembly(); }
    // Verify Assembly.GetName after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetName_System_Reflection_AssemblyName() { _ = typeof(byte).Assembly.GetName(); }
    // Verify Assembly.get_FullName after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_get_FullName_System_String() { _ = typeof(byte).Assembly.FullName; }
    // Verify Assembly.get_Location after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_get_Location_System_String() { _ = typeof(byte).Assembly.Location; }
    // Verify Assembly.get_ImageRuntimeVersion after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_get_ImageRuntimeVersion_System_String() { _ = typeof(byte).Assembly.ImageRuntimeVersion; }
    // Verify Assembly.GetTypes after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type() { _ = typeof(byte).Assembly.GetTypes(); }
    // Verify Assembly.GetExportedTypes after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetExportedTypes_System_Type() { _ = typeof(byte).Assembly.GetExportedTypes(); }
    // Verify Assembly.GetForwardedTypes after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetForwardedTypes_System_Type() { _ = typeof(byte).Assembly.GetForwardedTypes(); }
    // Verify Assembly.GetType after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String() { _ = typeof(byte).Assembly.GetType("hello"); }
    // Verify Assembly.GetType after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String_System_Boolean() { _ = typeof(byte).Assembly.GetType("hello", true); }
    // Verify Assembly.GetModule after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetModule_System_Reflection_Module_System_String() { _ = typeof(byte).Assembly.GetModule("hello"); }
    // Verify Assembly.GetModules after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module() { _ = typeof(byte).Assembly.GetModules(); }
    // Verify Assembly.GetManifestResourceStream after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceStream_System_IO_Stream_System_String() { _ = typeof(byte).Assembly.GetManifestResourceStream("hello"); }
    // Verify Assembly.GetManifestResourceNames after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceNames_System_String() { _ = typeof(byte).Assembly.GetManifestResourceNames(); }
    // Verify Assembly.get_EntryPoint after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_Assembly_get_EntryPoint_System_Reflection_MethodInfo() { _ = typeof(byte).Assembly.EntryPoint; }
    // Verify AssemblyName.get_Name after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_AssemblyName_get_Name_System_String() { _ = default(AssemblyName)!.Name; }
    // Verify AssemblyName.get_Version after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_AssemblyName_get_Version_System_Version() { _ = default(AssemblyName)!.Version; }
    // Verify AssemblyName.get_CultureInfo after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Reflection_AssemblyName_get_CultureInfo_System_Globalization_CultureInfo() { _ = default(AssemblyName)!.CultureInfo; }
    // Verify Type.get_Assembly after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }
    // Verify Type.GetType after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }}
