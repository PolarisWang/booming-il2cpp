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

// Auto-generated hot-update skeletons for EnumParsingHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class EnumParsingHotUpdate
{
    // Purpose: Verify Enum.Format executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_Format_System_String_System_Type_System_Object_System_String() { }
    // Purpose: Verify Enum.GetName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_GetName_System_String_System_RuntimeType_System_UInt64() { }
    // Purpose: Verify Enum.GetName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_GetName_System_String_System_Type_System_Object() { }
    // Purpose: Verify Enum.GetNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_GetNames_System_String_System_Type() { }
    // Purpose: Verify Enum.GetValues executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_GetValues_System_Array_System_Type() { }
    // Purpose: Verify Enum.IsDefined executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_IsDefined_System_Boolean_System_Type_System_Object() { }
    // Purpose: Verify Enum.Parse executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String() { }
    // Purpose: Verify Enum.Parse executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String_System_Boolean() { }
    // Purpose: Verify Enum.ToString executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_ToString_System_String() { _ = DayOfWeek.Monday.ToString(); }
    // Purpose: Verify Enum.ToString executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::ToString:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_ToString_System_String_System_String() { }
    // Purpose: Verify Enum.TryParse executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Boolean_System_Object() { }
    // Purpose: Verify Enum.TryParse executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Object() { }}
