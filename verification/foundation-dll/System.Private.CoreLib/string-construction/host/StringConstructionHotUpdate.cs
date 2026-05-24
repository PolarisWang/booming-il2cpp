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

// Hot-update stubs for family/System.Private.CoreLib/string/construction
public static class StringConstructionHotUpdate
{
    // Verify String..ctor after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ctor_System_Void_System_Char() { new String(Array.Empty<System.Char>()); }
    // Verify String..ctor after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::.ctor:System.Void(System.Char,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ctor_System_Void_System_Char_System_Int32() { new String('A', 42); }
    // Verify String..ctor after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ctor_System_Void_System_Char_System_Int32_System_Int32() { new String(Array.Empty<System.Char>(), 42, 42); }
    // Verify String.IsNullOrEmpty after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IsNullOrEmpty:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IsNullOrEmpty_System_Boolean_System_String() { _ = String.IsNullOrEmpty("hello"); }
    // Verify String.IsNullOrWhiteSpace after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IsNullOrWhiteSpace:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IsNullOrWhiteSpace_System_Boolean_System_String() { _ = String.IsNullOrWhiteSpace("hello"); }
    // Verify String.Copy after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Copy:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Copy_System_String_System_String() { _ = String.Copy("hello"); }
    // Verify String.Intern after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Intern:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Intern_System_String_System_String() { _ = String.Intern("hello"); }
    // Verify String.IsInterned after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IsInterned:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringConstruction)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IsInterned_System_String_System_String() { _ = String.IsInterned("hello"); }}
