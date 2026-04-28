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

// Hot-update stubs for family/System.Private.CoreLib/string/char-text-core
public static class StringCharTextCoreHotUpdate
{
    // Verify String.IndexOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String() { _ = "hello".IndexOf("hello"); }
    // Verify String.IndexOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char() { _ = "hello".IndexOf('A'); }
    // Verify String.Substring after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Substring_System_String_System_Int32() { _ = "hello".Substring(1); }
    // Verify String.Substring after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32() { _ = "hello".Substring(1, 2); }
    // Verify String.Compare after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String() { _ = String.Compare("hello", "hello"); }
    // Verify String.Compare after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I() { _ = string.Compare("hello", 0, "world", 0, 3); }
    // Verify String.Concat after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String() { _ = String.Concat("hello", "hello"); }
    // Verify String.Concat after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String() { _ = String.Concat("hello", "hello", "hello"); }
    // Verify String.Format after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object() { _ = String.Format("hello", Array.Empty<System.Object>()); }
    // Verify String.StartsWith after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String() { _ = "hello".StartsWith("hello"); }
    // Verify String.Contains after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String() { _ = "hello".Contains("hello"); }
    // Verify String.Replace after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String() { _ = "hello".Replace("hello", "hello"); }
    // Verify String.Split after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Split_System_String_System_Char() { _ = "hello".Split('A'); }
    // Verify String.ToUpper after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ToUpper_System_String() { _ = "hello".ToUpper(); }
    // Verify String.ToLower after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ToLower_System_String() { _ = "hello".ToLower(); }
    // Verify String.Trim after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Trim_System_String() { _ = "hello".Trim(); }
    // Verify String.Join after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String() { _ = String.Join("hello", Array.Empty<System.String>()); }
    // Verify Char.IsDigit after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char() { _ = Char.IsDigit('A'); }
    // Verify Char.IsLetter after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char() { _ = Char.IsLetter('A'); }
    // Verify Char.IsWhiteSpace after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char() { _ = Char.IsWhiteSpace('A'); }}
