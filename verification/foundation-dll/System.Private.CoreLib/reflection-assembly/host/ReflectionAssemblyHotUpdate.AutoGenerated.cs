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

// Auto-generated hot-update skeletons for ReflectionAssemblyHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class ReflectionAssemblyHotUpdate
{
    // Purpose: Verify Assembly.GetExecutingAssembly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetExecutingAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetExecutingAssembly(); }
    // Purpose: Verify Assembly.GetCallingAssembly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetCallingAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetCallingAssembly(); }
    // Purpose: Verify Assembly.GetEntryAssembly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetEntryAssembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetEntryAssembly(); }
    // Purpose: Verify Assembly.GetName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetName_System_Reflection_AssemblyName() { _ = typeof(byte).Assembly.GetName(); }
    // Purpose: Verify Assembly.get_FullName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_get_FullName_System_String() { _ = typeof(byte).Assembly.FullName; }
    // Purpose: Verify Assembly.get_Location executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_get_Location_System_String() { _ = typeof(byte).Assembly.Location; }
    // Purpose: Verify Assembly.get_ImageRuntimeVersion executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_get_ImageRuntimeVersion_System_String() { _ = typeof(byte).Assembly.ImageRuntimeVersion; }
    // Purpose: Verify Assembly.GetTypes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetTypes_System_Type() { _ = typeof(byte).Assembly.GetTypes(); }
    // Purpose: Verify Assembly.GetExportedTypes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetExportedTypes_System_Type() { _ = typeof(byte).Assembly.GetExportedTypes(); }
    // Purpose: Verify Assembly.GetForwardedTypes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetForwardedTypes_System_Type() { _ = typeof(byte).Assembly.GetForwardedTypes(); }
    // Purpose: Verify Assembly.GetType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String() { _ = typeof(byte).Assembly.GetType("hello"); }
    // Purpose: Verify Assembly.GetType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetType_System_Type_System_String_System_Boolean() { _ = typeof(byte).Assembly.GetType("hello", true); }
    // Purpose: Verify Assembly.GetModule executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetModule_System_Reflection_Module_System_String() { _ = typeof(byte).Assembly.GetModule("hello"); }
    // Purpose: Verify Assembly.GetModules executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetModules_System_Reflection_Module() { _ = typeof(byte).Assembly.GetModules(); }
    // Purpose: Verify Assembly.GetManifestResourceStream executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceStream_System_IO_Stream_System_String() { _ = typeof(byte).Assembly.GetManifestResourceStream("hello"); }
    // Purpose: Verify Assembly.GetManifestResourceNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_GetManifestResourceNames_System_String() { _ = typeof(byte).Assembly.GetManifestResourceNames(); }
    // Purpose: Verify Assembly.get_EntryPoint executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_Assembly_get_EntryPoint_System_Reflection_MethodInfo() { _ = typeof(byte).Assembly.EntryPoint; }
    // Purpose: Verify AssemblyName.get_Name executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_AssemblyName_get_Name_System_String() { _ = AssemblyName.Name; }
    // Purpose: Verify AssemblyName.get_Version executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_AssemblyName_get_Version_System_Version() { _ = AssemblyName.Version; }
    // Purpose: Verify AssemblyName.get_CultureInfo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_AssemblyName_get_CultureInfo_System_Globalization_CultureInfo() { _ = AssemblyName.CultureInfo; }
    // Purpose: Verify Type.get_Assembly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly() { _ = typeof(byte).Assembly; }
    // Purpose: Verify Type.GetType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionAssembly)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }}
