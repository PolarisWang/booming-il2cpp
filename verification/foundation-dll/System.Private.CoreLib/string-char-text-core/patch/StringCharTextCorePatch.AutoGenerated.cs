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

// Auto-generated patch-side skeletons for StringCharTextCorePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class StringCharTextCorePatch
{
    // Purpose: Verify String.IndexOf executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String() { _ = "hello".IndexOf("hello"); }
    // Purpose: Verify String.IndexOf executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char() { _ = "hello".IndexOf('A'); }
    // Purpose: Verify String.Substring executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Substring_System_String_System_Int32() { _ = "hello".Substring(1); }
    // Purpose: Verify String.Substring executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32() { _ = "hello".Substring(1, 2); }
    // Purpose: Verify String.Compare executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String() { _ = String.Compare("hello", "hello"); }
    // Purpose: Verify String.Compare executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I() { _ = string.Compare("hello", 0, "world", 0, 3); }
    // Purpose: Verify String.Concat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String() { _ = String.Concat("hello", "hello"); }
    // Purpose: Verify String.Concat executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String() { _ = String.Concat("hello", "hello", "hello"); }
    // Purpose: Verify String.Format executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object() { _ = String.Format("hello", Array.Empty<System.Object>()); }
    // Purpose: Verify String.StartsWith executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String() { _ = "hello".StartsWith("hello"); }
    // Purpose: Verify String.Contains executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String() { _ = "hello".Contains("hello"); }
    // Purpose: Verify String.Replace executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String() { _ = "hello".Replace("hello", "hello"); }
    // Purpose: Verify String.Split executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Split_System_String_System_Char() { _ = "hello".Split('A'); }
    // Purpose: Verify String.ToUpper executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_ToUpper_System_String() { _ = "hello".ToUpper(); }
    // Purpose: Verify String.ToLower executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_ToLower_System_String() { _ = "hello".ToLower(); }
    // Purpose: Verify String.Trim executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Trim_System_String() { _ = "hello".Trim(); }
    // Purpose: Verify String.Join executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String() { _ = String.Join("hello", Array.Empty<System.String>()); }
    // Purpose: Verify Char.IsDigit executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char() { _ = Char.IsDigit('A'); }
    // Purpose: Verify Char.IsLetter executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char() { _ = Char.IsLetter('A'); }
    // Purpose: Verify Char.IsWhiteSpace executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char() { _ = Char.IsWhiteSpace('A'); }}
