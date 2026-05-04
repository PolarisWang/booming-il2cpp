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

// Auto-generated skeleton for String And Char Text Core (family/System.Private.CoreLib/string/char-text-core).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class StringCharTextCore
{
    // String.IndexOf
    [MethodSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String()
    {
    var result = "hello".IndexOf("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String() { _ = "hello".IndexOf("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_String() { _ = "hello".IndexOf("hello"); }

    // String.IndexOf
    [MethodSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char()
    {
    var result = "hello".IndexOf('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char() { _ = "hello".IndexOf('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_IndexOf_System_Int32_System_Char() { _ = "hello".IndexOf('A'); }

    // String.Substring
    [MethodSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Substring_System_String_System_Int32()
    {
    var result = "hello".Substring(1);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32() { _ = "hello".Substring(1); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Substring_System_String_System_Int32() { _ = "hello".Substring(1); }

    // String.Substring
    [MethodSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32()
    {
    var result = "hello".Substring(1, 2);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32() { _ = "hello".Substring(1, 2); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Substring_System_String_System_Int32_System_Int32() { _ = "hello".Substring(1, 2); }

    // String.Compare
    [MethodSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String()
    {
    var result = String.Compare("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String() { _ = String.Compare("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_String() { _ = String.Compare("hello", "hello"); }

    // String.Compare
    [MethodSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I()
    {
    var result = string.Compare("hello", 0, "world", 0, 3);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I() { _ = string.Compare("hello", 0, "world", 0, 3); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Compare_System_Int32_System_String_System_Int32_System_String_System_Int32_System_I() { _ = string.Compare("hello", 0, "world", 0, 3); }

    // String.Concat
    [MethodSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String()
    {
    var result = String.Concat("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String() { _ = String.Concat("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String() { _ = String.Concat("hello", "hello"); }

    // String.Concat
    [MethodSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String()
    {
    var result = String.Concat("hello", "hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String() { _ = String.Concat("hello", "hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Concat_System_String_System_String_System_String_System_String() { _ = String.Concat("hello", "hello", "hello"); }

    // String.Format
    [MethodSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object()
    {
    var result = String.Format("hello", Array.Empty<System.Object>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object() { _ = String.Format("hello", Array.Empty<System.Object>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Format_System_String_System_String_System_Object() { _ = String.Format("hello", Array.Empty<System.Object>()); }

    // String.StartsWith
    [MethodSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String()
    {
    var result = "hello".StartsWith("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String() { _ = "hello".StartsWith("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_StartsWith_System_Boolean_System_String() { _ = "hello".StartsWith("hello"); }

    // String.Contains
    [MethodSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String()
    {
    var result = "hello".Contains("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String() { _ = "hello".Contains("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Contains_System_Boolean_System_String() { _ = "hello".Contains("hello"); }

    // String.Replace
    [MethodSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String()
    {
    var result = "hello".Replace("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String() { _ = "hello".Replace("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Replace_System_String_System_String_System_String() { _ = "hello".Replace("hello", "hello"); }

    // String.Split
    [MethodSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Split_System_String_System_Char()
    {
    var result = "hello".Split('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Split_System_String_System_Char() { _ = "hello".Split('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Split_System_String_System_Char() { _ = "hello".Split('A'); }

    // String.ToUpper
    [MethodSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_ToUpper_System_String()
    {
    var result = "hello".ToUpper();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_ToUpper_System_String() { _ = "hello".ToUpper(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ToUpper_System_String() { _ = "hello".ToUpper(); }

    // String.ToLower
    [MethodSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_ToLower_System_String()
    {
    var result = "hello".ToLower();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_ToLower_System_String() { _ = "hello".ToLower(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToLower:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_ToLower_System_String() { _ = "hello".ToLower(); }

    // String.Trim
    [MethodSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Trim_System_String()
    {
    var result = "hello".Trim();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Trim_System_String() { _ = "hello".Trim(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Trim_System_String() { _ = "hello".Trim(); }

    // String.Join
    [MethodSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String()
    {
    var result = String.Join("hello", Array.Empty<System.String>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String() { _ = String.Join("hello", Array.Empty<System.String>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_String_Join_System_String_System_String_System_String() { _ = String.Join("hello", Array.Empty<System.String>()); }

    // Char.IsDigit
    [MethodSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char()
    {
    var result = Char.IsDigit('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char() { _ = Char.IsDigit('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsDigit_System_Boolean_System_Char() { _ = Char.IsDigit('A'); }

    // Char.IsLetter
    [MethodSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char()
    {
    var result = Char.IsLetter('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char() { _ = Char.IsLetter('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsLetter_System_Boolean_System_Char() { _ = Char.IsLetter('A'); }

    // Char.IsWhiteSpace
    [MethodSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char()
    {
    var result = Char.IsWhiteSpace('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char() { _ = Char.IsWhiteSpace('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Char_IsWhiteSpace_System_Boolean_System_Char() { _ = Char.IsWhiteSpace('A'); }
}
